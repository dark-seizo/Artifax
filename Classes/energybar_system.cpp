#include "energybar_system.h"

void EnergyBarSystem::configure(EventManager &events)
{
	events.subscribe<AttemptWeaponFireEvent>(*this);
	eventManager = &events;
}

void EnergyBarSystem::receive(const AttemptWeaponFireEvent &attemptWeaponFireEvent)
{
	auto entity = attemptWeaponFireEvent.entity;

	if (entity.has_component<EnergyBarComponent>() && entity.has_component<WeaponComponent>())
	{
		auto energyBarComponent = entity.component<EnergyBarComponent>();
		auto displayEntity = energyBarComponent->displayEntity;
		auto weaponComponent = entity.component<WeaponComponent>();
		//auto progressBar = displayEntity.component<ProgressBarComponent>()->progressBar;

		float totalCost = 0.f; 

		if (weaponComponent->laser.isReady() && weaponComponent->laser.level != LaserWeapon::LASER_INACTIVE) totalCost += PLAYER_LASER_ENERGY_COST;
		if (weaponComponent->missile.isReady() && weaponComponent->missile.level != MissileWeapon::MISSILE_INACTIVE)  totalCost += PLAYER_MISSILE_ENERGY_COST;
		//if (weaponComponent->turretDelay <= 0) totalCost += PLAYER_TURRET_FIRE_ENERGY_COST;

		if (totalCost > 0.f && !energyBarComponent->depleted)
		{
			//reset recharge time left
			energyBarComponent->rechargeTimeLeft = energyBarComponent->rechargeDelay;
			energyBarComponent->energyLevel -= totalCost;
			eventManager->emit<PlayerWeaponFiredEvent>(entity);

			if (energyBarComponent->energyLevel <= 0)
			{
				energyBarComponent->depleted = true;
				//add 50% more delay before the recharge begins
				energyBarComponent->rechargeTimeLeft += energyBarComponent->rechargeDelay / 2;
			}
			//progressBar->setPercentage(energyBarComponent->energyLevel);
		}
	}
}

void EnergyBarSystem::update(EntityManager &es, EventManager &events, double dt)
{
	EnergyBarComponent::Handle energyBarComponent;
	for (Entity entity : es.entities_with_components(energyBarComponent))
	{
		if (energyBarComponent->rechargeTimeLeft > 0)
		{
			energyBarComponent->rechargeTimeLeft -= dt;
		}
		else if (energyBarComponent->rechargeTimeLeft <= 0 && energyBarComponent->energyLevel < energyBarComponent->maxEnergyLevel)
		{
			//increase recharge time by 50% if depleted
			energyBarComponent->energyLevel += energyBarComponent->rechargeRate * dt * (energyBarComponent->depleted ? 1.5f : 1.0f);

			//check to see if the depleted flag needs to be set
			if (energyBarComponent->energyLevel >= energyBarComponent->maxEnergyLevel && energyBarComponent->depleted)
			{
				energyBarComponent->depleted = false;
			}
		}
		auto progressBar = energyBarComponent->displayEntity.component<ProgressBarComponent>()->progressBar;
		progressBar->setPercentage(energyBarComponent->energyLevel);
	}
}

