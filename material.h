#ifndef MATERIAL_H
#define MATERIAL_H

class material {
public:
  virtual ~material() {}
  virtual bool scatter(
    const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
  ) const = 0;
};

class metal : public material {
public:
  metal(const color& a) : albedo(a) {}
  metal(const color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

  virtual bool scatter(
    const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
  ) const {
    vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
    scattered = ray(rec.p, reflected + fuzz*random_in_unit_sphere());
    attenuation = albedo;
    return (dot(scattered.direction(), rec.normal) > 0);
  }

public:
  color albedo;
  double fuzz;
};

class dielectric : public material {
public:
  dielectric(double ri) : ref_idx(ri) {}

  virtual bool scatter(
    const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
  ) const {
    attenuation = color(1.0, 1.0, 1.0);
    double etai_over_etat;
    if (rec.front_face) {
      etai_over_etat = 1.0 / ref_idx;
    } else {
      etai_over_etat = ref_idx;
    }

    vec3 unit_direction = unit_vector(r_in.direction());
    double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
    double sin_theta = sqrt(1.0 - cos_theta*cos_theta);
    if (etai_over_etat * sin_theta > 1.0 ) {
      vec3 reflected = reflect(unit_direction, rec.normal);
      scattered = ray(rec.p, reflected);
      return true;
    }

    vec3 refracted = refract(unit_direction, rec.normal, etai_over_etat);
    scattered = ray(rec.p, refracted);
    return true;
  }

  double ref_idx;
};

#endif
