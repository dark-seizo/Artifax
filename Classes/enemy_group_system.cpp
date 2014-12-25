#include "enemy_group_system.h"

void EnemyGroupSystem::configure(EventManager &events)
{

}
void EnemyGroupSystem::update(EntityManager &es, EventManager &events, double dt)
{
	EnemyGroupComponent::Handle enemyGroupComponent;
	UpgradeComponent::Handle upgradeComponent;
	for (auto entity : es.entities_with_components(enemyGroupComponent, upgradeComponent))
	{
		if (enemyGroupComponent->groupEnemies.empty())
		{
			entity.destroy();
		}
		else
		{
			//iterate over groupEnemies and remove the invalid ones
			std::list<entityx::Entity>::iterator iter = enemyGroupComponent->groupEnemies.begin();
			std::list<entityx::Entity>::iterator end = enemyGroupComponent->groupEnemies.end();
			while (iter != end)
			{
				if (!iter->valid())
				{
					//remove from list and increment iterator to keep from having issues with the removed element
					enemyGroupComponent->groupEnemies.erase(iter++);
				}
                else
                    ++iter;
			}

			if (enemyGroupComponent->groupEnemies.size() == 1)
			{
				//set that enemies upgrade drop to the last remaining enemy, then destroy the group entity
                if (enemyGroupComponent->groupEnemies.front().has_component<UpgradeComponent>())
                {
                    if (upgradeComponent->drop != UpgradeDrop::NO_UPGRADE)
                    {
                        auto drop = upgradeComponent->drop;
                        enemyGroupComponent->groupEnemies.front().component<UpgradeComponent>()->drop = drop;
                    }
                    //entity.destroy();
                }
				//destroy the group
				entity.destroy();
			}
		}
	}
}
