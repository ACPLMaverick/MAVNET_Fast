#pragma once

namespace Rendering
{
	class Camera
	{
	public:

		Camera();
		virtual ~Camera();

		void Initialize
		(
			const glm::vec3* position,
			const glm::vec3* target,
			float fov,
			float dimension,
			float nearPlane,
			float farPlane
		);
		void Shutdown();
		void Update();

		void SetPosition(const glm::vec3* position);
		void SetPositionKeepTarget(const glm::vec3* position);
		void SetTarget(const glm::vec3* target);
		void SetDirection(const glm::vec3* direction);
		void SetFov(float fov);
		void SetDimension(float dimension);
		void SetNear(float nearPlane);
		void SetFar(float farPlane);

		const glm::mat4* GetView() const { return &_view; }
		const glm::mat4* GetProj() const { return &_proj; }
		const glm::mat4* GetViewProj() const { return &_viewProj; }

		const glm::vec3* GetPosition() const { return &_position; }
		const glm::vec3* GetTarget() const { return &_target; }
		const glm::vec3* GetDirection() const { return &_direction; }

		float GetFov() const { return _fov; }
		float GetDimension() const { return _dimension; }
		float GetNear() const { return _near; }
		float GetFar() const { return _far; }

		bool IsViewNeedUpdate() const { return _bUpdateView; }
		bool IsProjNeedUpdate() const { return _bUpdateProj; }
		bool IsViewProjNeedUpdate() const { return _bUpdateView || _bUpdateProj; }

	protected:

		void CalcDirection();
		void CalcTarget();
		void UpdateView();
		void UpdateProj();
		void UpdateViewProj();

		static const float ARBITRARY_DIRECTION_TARGET_DISTANCE;

		glm::mat4 _view;
		glm::mat4 _proj;
		glm::mat4 _viewProj;

		glm::vec3 _position;
		glm::vec3 _target;
		glm::vec3 _direction;

		float _fov;
		float _dimension;
		float _near;
		float _far;

		bool _bUpdateView;
		bool _bUpdateProj;

	};
}