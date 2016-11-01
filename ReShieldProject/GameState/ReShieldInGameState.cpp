#include "GameState/ReShieldInGameState.hpp"

#include <vector>
#include "SaveSystem/SaveSystem.hpp"

#include "Types/Types.hpp"
#include "Core/CameraGameObject.hpp"
#include "Core/LightGameObject.hpp"
#include "Core/StateSharedData.hpp"
#include "Core/CameraComponent.hpp"
#include "Core/TransformComponent.hpp"
#include "Core/LightComponent.hpp"
#include "Transform/Transform.hpp"
#include "Camera/Camera.hpp"
#include "Light/Light.hpp"
#include "Camera/PerspectiveCamera.hpp"
#include "Input/Input.hpp"

#include "Core/GraphicGameObject.hpp"

#include "imgui.h"

namespace ReShield
{
	//class ReShieldInGameStateData
	//{

	//};

	using namespace Eternal::SaveSystem;
	using namespace Eternal::Types;
	using namespace Eternal::Input;
	using namespace Eternal::Components;

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
		PerspectiveCamera* Camera = new PerspectiveCamera(1.f, 2000.0f, 90.0f, 16.0f/9.0f);
		GetSharedData()->Camera->GetCameraComponent()->SetCamera(Camera);
		GetSharedData()->Camera->GetCameraComponent()->AttachTo(GetSharedData()->Camera->GetTransformComponent());

		GetSharedData()->Lights = new LightGameObject();
		Light* PointLight = new Light(Vector3(1.0f, 0.5f, 80.0f / 255.0f), 100.0f, 1.0f);
		GetSharedData()->Lights->GetLightComponent()->SetLight(PointLight);
		GetSharedData()->Lights->GetTransformComponent()->GetTransform().SetTranslation(Vector3(0.f, 150.0f, 0.f));

		//(*GetSharedData()->GraphicGameObjects)[0]->GetTransformComponent()->GetTransform().Rotate(Vector3(-90.0f, 0.f, 0.f));
	}
	void ReShieldInGameState::Update()
	{
		TimeSecondsT ElapsedTime = Eternal::Time::Time::Get()->GetDeltaTimeSeconds();

		Camera* MainCamera = GetSharedData()->Camera->GetCameraComponent()->GetCamera();
		Transform& CameraTransform = GetSharedData()->Camera->GetTransformComponent()->GetTransform();

		//Vector4 Pos0(0.f, 0.f, 1.f, 1.f);
		//Vector4 Pos1(0.f, 0.f, 1000.f, 1.f);

		//Vector4 Pos2(1.f, 1.f, 1.f, 1.f);
		//Vector4 Pos3(1.f, 1.f, 1000.f, 1.f);

		//Vector4 Pos4(1000.f, 1000.f, 1.f, 1.f);
		//Vector4 Pos5(1000.f, 1000.f, 1000.f, 1.f);

		//Matrix4x4 View;
		//Matrix4x4 Proj;
		//Matrix4x4 InvProj;
		//MainCamera->GetViewProjectionMatrix(Proj);
		//MainCamera->GetViewProjectionMatrixInverse(InvProj);
		//MainCamera->GetViewMatrix(View);

		//Vector4 ProjPos0 = Proj * Pos0;
		//Vector4 ProjPos1 = Proj * Pos1;
		//Vector4 ProjPos2 = Proj * Pos2;
		//Vector4 ProjPos3 = Proj * Pos3;
		//Vector4 ProjPos4 = Proj * Pos4;
		//Vector4 ProjPos5 = Proj * Pos5;

		//Vector4 WSCube[8];
		//Vector4 ProjCube[8];
		//for (int x = 0; x < 2; ++x)
		//{
		//	float X = x ? 1.f : -1.f;
		//	for (int y = 0; y < 2; ++y)
		//	{
		//		float Y = y ? 1.f : -1.f;
		//		for (int z = 0; z < 2; ++z)
		//		{
		//			float Z = z ? 1.f : -1.f;
		//			WSCube[(x * 2 + y) * 2 + z] = Vector4(X, Y, Z, 1.f);
		//			ProjCube[(x * 2 + y) * 2 + z] = Proj * WSCube[(x * 2 + y) * 2 + z];
		//		}
		//	}
		//}

		//Vector4 PosFarSS(-1.f, -1.f, 1.f, 1.f);
		//Vector4 PosNearSS(-1.f, -1.f, 0.f, 1.f);
		//Vector4 PosFarWS = InvProj * PosFarSS;
		//PosFarWS.x /= PosFarWS.w;
		//PosFarWS.y /= PosFarWS.w;
		//PosFarWS.z /= PosFarWS.w;
		//PosFarWS.w /= PosFarWS.w;
		//Vector4 PosNearWS = InvProj * PosNearSS;
		//PosNearWS.x /= PosNearWS.w;
		//PosNearWS.y /= PosNearWS.w;
		//PosNearWS.z /= PosNearWS.w;
		//PosNearWS.w /= PosNearWS.w;
		//Vector4 PosNearSS2 = Proj * PosNearWS;
		//PosNearSS2.x /= PosNearSS2.w;
		//PosNearSS2.y /= PosNearSS2.w;
		//PosNearSS2.z /= PosNearSS2.w;
		//PosNearSS2.w /= PosNearSS2.w;
		//Vector4 PosFarSS2 = Proj * PosFarWS;
		//PosFarSS2.x /= PosFarSS2.w;
		//PosFarSS2.y /= PosFarSS2.w;
		//PosFarSS2.z /= PosFarSS2.w;
		//PosFarSS2.w /= PosFarSS2.w;

		//Vector4 TestSS(0.35437f, -0.84556f, 0.76429f, 1.0f);
		//InvProj = Matrix4x4(
		//	1.777778f, 0.f, 0.f, 0.f,
		//	0.f, 1.0f, 0.f, 0.f,
		//	2.67384f, -17.96933f, -322.0237f, -0.99899f,
		//	-2.67652f, 17.98732f, 323.346f, 0.999999f
		//	);
		//Vector4 TestWS = InvProj * TestSS;
		//TestWS.x /= TestWS.w;
		//TestWS.y /= TestWS.w;
		//TestWS.z /= TestWS.w;
		//TestWS.w /= TestWS.w;

		//Vector4 VecFwSS(0.f, 0.f, 1.f, 0.f);
		//Vector4 VecFwWS = View * VecFwSS;

		Vector3 Forward = MainCamera->GetForward();
		Vector3 Up = MainCamera->GetUp();
		Vector3 Right = MainCamera->GetRight();

		const float ForwardSpeed = (float)ElapsedTime * 500.0f * Eternal::Input::Input::Get()->GetAxis(Eternal::Input::Input::JOY0_LY);
		//const float UpSpeed = (float)ElapsedTime * 100.0f * 0.f;
		const float UpSpeed = (float)ElapsedTime * 500.0f * Eternal::Input::Input::Get()->GetAxis(Eternal::Input::Input::JOY0_RY);
		const float RightSpeed = (float)ElapsedTime * 500.0f * Eternal::Input::Input::Get()->GetAxis(Eternal::Input::Input::JOY0_LX);

		const Vector3 Speed = Forward * ForwardSpeed + Right * RightSpeed + Up * UpSpeed;

		//const float RotateX = (float)ElapsedTime * 10.0f * Eternal::Input::Input::Get()->GetAxis(Eternal::Input::Input::JOY0_RY);
		//const float RotateY = (float)ElapsedTime * 10.0f * Eternal::Input::Input::Get()->GetAxis(Eternal::Input::Input::JOY0_RX);
		//const float RotateZ = (float)ElapsedTime * 10.0f * 0.f;

		//const Vector3 AngularSpeed = Vector3(RotateX, RotateY, RotateZ);

		const Vector3 AngularSpeed = Vector3(0.f, 0.f, 0.f);

		CameraTransform.Rotate(AngularSpeed);
		CameraTransform.Translate(Vector3(RightSpeed, UpSpeed, ForwardSpeed));

		Vector3 Position = CameraTransform.GetTranslation();
		Vector4 Rotation = CameraTransform.GetRotation();

		ImGui::Begin("Debug camera position");
		//ImGui::Text("Position: [%f, %f, %f]", Position.x, Position.y, Position.z);
		ImGui::InputFloat3("Position", &Position.x);
		ImGui::InputFloat4("Rotation", &Rotation.x);
		ImGui::Text("Forward: [%f %f %f]", Forward.x, Forward.y, Forward.z);
		ImGui::Text("Right: [%f %f %f]", Right.x, Right.y, Right.z);
		ImGui::Text("Up: [%f %f %f]", Up.x, Up.y, Up.z);
		ImGui::Text("Speed: [%f, %f, %f]", Speed.x, Speed.y, Speed.z);
		ImGui::End();

		CameraTransform.SetTranslation(Position);

		float LightDistance = GetSharedData()->Lights->GetLightComponent()->GetLight()->GetDistance();
		float LightIntensity = GetSharedData()->Lights->GetLightComponent()->GetLight()->GetIntensity();
		Vector3 LightColor = GetSharedData()->Lights->GetLightComponent()->GetLight()->GetColor();
		Vector3 LightPosition = GetSharedData()->Lights->GetTransformComponent()->GetTransform().GetTranslation();

		ImGui::Begin("Light settings");
		ImGui::InputFloat3("Light position", &LightPosition.x);
		ImGui::SliderFloat("Light distance", &LightDistance, 0.001f, 1000.0f);
		ImGui::SliderFloat("Light intensity", &LightIntensity, 0.001f, 50.0f);
		ImGui::ColorEdit3("Light color", &LightColor.x);
		ImGui::End();

		GetSharedData()->Lights->GetTransformComponent()->GetTransform().SetTranslation(LightPosition);
		GetSharedData()->Lights->GetLightComponent()->GetLight()->SetDistance(LightDistance);
		GetSharedData()->Lights->GetLightComponent()->GetLight()->SetIntensity(LightIntensity);
		GetSharedData()->Lights->GetLightComponent()->GetLight()->SetColor(LightColor);
	}
	GameState* ReShieldInGameState::NextState()
	{
		return this;
	}
	void ReShieldInGameState::End()
	{

	}
}

