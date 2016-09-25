#include "GameState/ReShieldInGameState.hpp"

#include <vector>
#include "SaveSystem/SaveSystem.hpp"

#include "Core/CameraGameObject.hpp"
#include "Core/StateSharedData.hpp"
#include "Core/CameraGameObject.hpp"
#include "Types/Types.hpp"
#include "Core/CameraComponent.hpp"
#include "Core/TransformComponent.hpp"
#include "Transform/Transform.hpp"
#include "Camera/Camera.hpp"
#include "Camera/PerspectiveCamera.hpp"
#include "Input/Input.hpp"

#include "imgui.h"

namespace ReShield
{
	//class ReShieldInGameStateData
	//{

	//};

	using namespace Eternal::SaveSystem;
	using namespace Eternal::Types;
	using namespace Eternal::Input;

	ReShieldInGameState::ReShieldInGameState()
	{
		//_ReShieldInGameStateData = new ReShieldInGameStateData();
	}

	ReShieldInGameState::~ReShieldInGameState()
	{
		//delete _ReShieldInGameStateData;
		//_ReShieldInGameStateData = nullptr;
	}

	void ReShieldInGameState::Begin()
	{
		GetSharedData()->GraphicGameObjects = (std::vector<GraphicGameObject*>*)Eternal::SaveSystem::SaveSystem::Get()->Load("save.sav");
		GetSharedData()->Camera = new CameraGameObject();
		PerspectiveCamera* Camera = new PerspectiveCamera(1.f, 1000.0f, 90.0f, 16.0f/9.0f);

		GetSharedData()->Camera->GetCameraComponent()->SetCamera(Camera);
		GetSharedData()->Camera->GetCameraComponent()->AttachTo(GetSharedData()->Camera->GetTransformComponent());
	}
	void ReShieldInGameState::Update()
	{
		TimeSecondsT ElapsedTime = Eternal::Time::Time::Get()->GetDeltaTimeSeconds();

		Camera* MainCamera = GetSharedData()->Camera->GetCameraComponent()->GetCamera();
		Transform& CameraTransform = GetSharedData()->Camera->GetTransformComponent()->GetTransform();

		Vector4 Pos0(0.f, 0.f, 1.f, 1.f);
		Vector4 Pos1(0.f, 0.f, 1000.f, 1.f);

		Vector4 Pos2(1.f, 1.f, 1.f, 1.f);
		Vector4 Pos3(1.f, 1.f, 1000.f, 1.f);

		Vector4 Pos4(1000.f, 1000.f, 1.f, 1.f);
		Vector4 Pos5(1000.f, 1000.f, 1000.f, 1.f);

		Matrix4x4 Proj;
		MainCamera->GetViewProjectionMatrix(Proj);

		Vector4 ProjPos0 = Proj * Pos0;
		Vector4 ProjPos1 = Proj * Pos1;
		Vector4 ProjPos2 = Proj * Pos2;
		Vector4 ProjPos3 = Proj * Pos3;
		Vector4 ProjPos4 = Proj * Pos4;
		Vector4 ProjPos5 = Proj * Pos5;

		Vector4 WSCube[8];
		Vector4 ProjCube[8];
		for (int x = 0; x < 2; ++x)
		{
			float X = x ? 1.f : -1.f;
			for (int y = 0; y < 2; ++y)
			{
				float Y = y ? 1.f : -1.f;
				for (int z = 0; z < 2; ++z)
				{
					float Z = z ? 1.f : -1.f;
					WSCube[(x * 2 + y) * 2 + z] = Vector4(X, Y, Z, 1.f);
					ProjCube[(x * 2 + y) * 2 + z] = Proj * WSCube[(x * 2 + y) * 2 + z];
				}
			}
		}

		Vector3 Forward = MainCamera->GetForward();
		Vector3 Up = MainCamera->GetUp();
		Vector3 Right = MainCamera->GetRight();

		const float ForwardSpeed = (float)ElapsedTime * 50.0f * Eternal::Input::Input::Get()->GetAxis(Eternal::Input::Input::JOY0_LY);
		const float UpSpeed = (float)ElapsedTime * 10.0f * 0.f;
		const float RightSpeed = (float)ElapsedTime * 50.0f * Eternal::Input::Input::Get()->GetAxis(Eternal::Input::Input::JOY0_LX);

		const Vector3 Speed = Forward * ForwardSpeed + Right * RightSpeed + Up * UpSpeed;

		//const float RotateX = (float)ElapsedTime * 10.0f * Eternal::Input::Input::Get()->GetAxis(Eternal::Input::Input::JOY0_RY);
		//const float RotateY = (float)ElapsedTime * 10.0f * Eternal::Input::Input::Get()->GetAxis(Eternal::Input::Input::JOY0_RX);
		//const float RotateZ = (float)ElapsedTime * 10.0f * 0.f;

		//const Vector3 AngularSpeed = Vector3(RotateX, RotateY, RotateZ);

		const Vector3 AngularSpeed = Vector3(0.f, 0.f, 0.f);

		CameraTransform.Rotate(AngularSpeed);
		CameraTransform.Translate(Vector3(RightSpeed, UpSpeed, ForwardSpeed));

		const Vector3& Position = CameraTransform.GetTranslation();

		ImGui::Begin("Debug camera position");
		ImGui::Text("Position: [%f, %f, %f]", Position.x, Position.y, Position.z);
		ImGui::Text("Speed: [%f, %f, %f]", Speed.x, Speed.y, Speed.z);
		ImGui::End();
	}
	GameState* ReShieldInGameState::NextState()
	{
		return this;
	}
	void ReShieldInGameState::End()
	{

	}
}
