
#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <glm/glm.hpp>


class Camera
{
public:
	Camera(glm::vec3 _eye, glm::vec3 _center, glm::vec3 _up, float _sw, float _sh)
	{
		setInverseVertical(false);

		glm::vec3 e3 = _eye - _center;
		glm::vec3 e1 = glm::cross(_up, e3);
		glm::vec3 e2 = glm::cross(e3, e1);
			
		e1 = glm::normalize(e1);
		e2 = glm::normalize(e2);
		e3 = glm::normalize(e3);
	
		A = glm::mat3x3(e1, e2, e3);
		center = _center;
		anchor = glm::vec2(0.0f);
		scr = glm::vec2(_sw, _sh);
		ppm = 32;
	}

	void setInverseVertical(bool inverse) {
		float yy = inverse ? -1.0f : 1.0f;
		// матрица, меняющая знак компоненты y
		T = glm::mat2x2(1.0f, 0.0f, 0.0f, yy);
	}

	void move(const glm::vec3& _dv) {
		center += _dv;
	}

	void setCenter(const glm::vec3& _pos) {
		center = _pos;
	}

	glm::vec3 getCenter() const {
		return center;
	}

	void setAnchor(const glm::vec2& _anchor) {
		anchor = _anchor;
	}

	void setAnchorNormalized(const glm::vec2& _normAnchor) {
		anchor = scr * _normAnchor;
	}

	glm::vec2 getAnchor() const {
		return anchor;
	}

	void zoom(float _zoom) {
		ppm *= _zoom;
		ppm = glm::clamp(ppm, ppm_range.x, ppm_range.y);
	}

	void setPixelsPerMetreRange(float _ppm_min, float _ppm_max) {
		if( _ppm_max > _ppm_min && _ppm_min > 0 ) {
			ppm_range.x = _ppm_min;
			ppm_range.y = _ppm_max;
		}
	}

	void setPixelsPerMetre(float _ppm) {
		ppm = glm::clamp(_ppm, ppm_range.x, ppm_range.y);
	}

	glm::vec2 toScreen(const glm::vec3& _worldCoords) const	{
		// здесь всё в метрах
		glm::vec3 screen3d = glm::transpose(A) * (_worldCoords - center);
		// здесь в пикселях
		return T * glm::vec2(screen3d) * ppm + anchor;
	}

	glm::vec3 toWorld(const glm::vec2& _screenCoords) const	{
		glm::vec2 world2d = T * (_screenCoords - anchor) / ppm;
		return A * glm::vec3(world2d, 0.0f) + center;
	}

	float toMetres(float _pixels) const	{
		return _pixels / ppm;
	}

	float toPixels(float _metres) const	{
		return _metres * ppm;
	}
private:
	float ppm;
	glm::vec2 ppm_range;
	glm::vec2 scr;
	glm::vec2 anchor;
	glm::vec3 center;
	glm::mat3x3 A;
	glm::mat2x2 T;
};

#endif //__CAMERA_H__
