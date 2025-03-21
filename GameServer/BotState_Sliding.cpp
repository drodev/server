#include "stdafx.h"
#include "BotState_Sliding.h"
#include "char.h"
#include "GameServer.h"
#include "GameMain.h"


CBotState_Sliding::CBotState_Sliding(CCharacter* pParent)
	:CCharState(CHARSTATE_SLIDING, pParent)
{
	m_pParent->InitSliding();
}

CBotState_Sliding::~CBotState_Sliding()
{
}


void CBotState_Sliding::OnEnter()
{
	m_pParent->InitSliding();
	if (m_pParent->GetBotController())
	{
		m_pParent->GetBotController()->SetControlBlock(true);
	}

	CCharState::OnEnter();
}

void CBotState_Sliding::OnExit()
{
	if (m_pParent->GetBotController())
	{
		m_pParent->GetBotController()->SetControlBlock(false);
	}

	CCharState::OnExit();
}

int CBotState_Sliding::OnUpdate(DWORD dwTickDiff, float fMultiple)
{
	CCharState::OnUpdate(dwTickDiff, fMultiple);
	//((void(__fastcall *)(CBot *, _QWORD))v7->m_pParent->vfptr[4].__vecDelDtor)(v7->m_pParent, dwTickTime);
	//((void(__fastcall *)(CBot *, _QWORD))v7->m_pParent->vfptr[3].SendPacket)(v7->m_pParent, v8);

	m_pParent->IncSlidingTime(dwTickDiff);

	if (m_pParent->GetSlidingTime() >= NTL_BATTLE_SLIDING_END_TIME || (m_pParent->IsPC() == false && m_pParent->GetSlidingTime() >= NTL_BATTLE_SLIDING_END_TIME))
	{
		if (m_pParent->GetBuffManager()->CheckAndApplyOtherStun(CHARSTATE_SLIDING, INVALID_WORD) == false) //check if any previous stuns exist. If yes, then apply them and return true. Otherwise return false.
			m_pParent->SendCharStateStanding();
	}

	return 0;
}

int CBotState_Sliding::OnObjectMsg(CObjectMsg * pObjectMsg)
{
	return 0;
}

bool CBotState_Sliding::CanTransition(BYTE stateID)
{
	if (stateID == CHARSTATE_SITTING || stateID == CHARSTATE_RIDEON)
		return false;

	return true;
}

int CBotState_Sliding::CopyTo(sCHARSTATE_DETAIL* pStateDetail)
{
	return NULL;
}

void CBotState_Sliding::CopyFrom(sCHARSTATE_DETAIL* pStateDetail)
{
	// TODO: Is this correct?
	if (m_pParent->GetObjType() > OBJTYPE_PC)
	{
		if (m_pParent->GetCurWorld())
		{
			CGameServer* app = (CGameServer*)g_pApp;

			CNtlVector rvShift(pStateDetail->sCharStateSliding.vShift);
			CNtlVector rvDestLoc;
			CNtlVector rvNewDir;

			if (app->GetGameMain()->GetWorldManager()->GetDestLocAfterCollision(m_pParent->GetCurWorld(), m_pParent, rvShift, rvDestLoc, rvNewDir))
			{
				m_pParent->SetCurLoc(rvDestLoc, m_pParent->GetCurWorld());
				m_pParent->SetCurDir(rvNewDir);
			}
			/*else
			{
			ERR_LOG(LOG_USER,"GetDestLocAfterCollision() failed., pWorld->GetID() = %u, m_pParent->GetCurLoc() = (%f, %f, %f)");
			}*/
		}
		else ERR_LOG(LOG_USER, "The bot doesn't belong to any world.(NULL == pWorld)., m_pParent->GetID() = %u", m_pParent->GetID());
	}
}

