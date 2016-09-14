#include "GameState/ReShieldInGameState.hpp"

#include <vector>
#include "SaveSystem/SaveSystem.hpp"
#include "Core/GameObject.hpp"
#include "Core/StateSharedData.hpp"

namespace ReShield
{
	//class ReShieldInGameStateData
	//{

	//};

	using namespace Eternal::SaveSystem;

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
		GetSharedData()->GameObjects = (std::vector<GameObject*>*)SaveSystem::Get()->Load("save.sav");
	}
	void ReShieldInGameState::Update()
	{

	}
	GameState* ReShieldInGameState::NextState()
	{
		return this;
	}
	void ReShieldInGameState::End()
	{

	}
}
