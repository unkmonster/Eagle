#include "Frosbite.h"

#include "Logger.h"

bool fb::ClientSoldierEntity::GetBonePos(int BoneId, D3DXVECTOR3* vOut) {
	__try {
		BoneCollisionComponent* pBoneCollisionComponent = this->bonecollisioncomponent;
		if (!ValidPointer(pBoneCollisionComponent)) return false;

		if (!pBoneCollisionComponent->m_ragdollTransforms.m_ValidTransforms) return false;

		fb::UpdatePoseResultData::QuatTransform* pQuat = pBoneCollisionComponent->m_ragdollTransforms.m_ActiveWorldTransforms;
		if (!ValidPointer(pQuat)) return false;

		vOut->x = pQuat[BoneId].m_TransAndScale.x;
		vOut->y = pQuat[BoneId].m_TransAndScale.y;
		vOut->z = pQuat[BoneId].m_TransAndScale.z;
		//vOut->w = pQuat[BoneId].m_TransAndScale.w;

		return true;
	} __except (EXCEPTION_EXECUTE_HANDLER) {
		SPDLOG_WARN("EX: {}", GetExceptionCode());
		return false;	
	}
}

bool fb::ClientSoldierEntity::GetHealth(std::pair<float, float>& hp) const {
	__try {
		hp.first = this->healthcomponent->m_Health;
		hp.second = this->healthcomponent->m_MaxHealth;
		return true;
	} __except (EXCEPTION_EXECUTE_HANDLER) {
		return false;
	}	
}

std::optional<Vec3> fb::ClientPlayer::GetPosition() const {
	if (!ValidPointer(this->clientSoldierEntity)) 
		return std::nullopt;

	Vec3 result{};
	if (this->InVehicle()) {
		Matrix16 matrix;
		this->clientVehicleEntity->GetTransform(&matrix);
		result = matrix.data[3];
	} else {
		result = this->clientSoldierEntity->location;
	}
	return result;
}
