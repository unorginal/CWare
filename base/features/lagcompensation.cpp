#include "lagcompensation.h"

void CLagCompensation::Run(CUserCmd* pCmd)
{
	/*
	 * we have much public info for that
	 * now it is your own way gl
	 */
}

void CLagCompensation::UpdateIncomingSequences(INetChannel* pNetChannel)
{
	if (pNetChannel == nullptr)
		return;

	// set to real sequence to update, otherwise needs time to get it work again
	if (nLastIncomingSequence == 0)
		nLastIncomingSequence = pNetChannel->iInSequenceNr;

	// check how much sequences we can spike
	if (pNetChannel->iInSequenceNr > nLastIncomingSequence)
	{
		nLastIncomingSequence = pNetChannel->iInSequenceNr;
		vecSequences.emplace_front(SequenceObject_t(pNetChannel->iInReliableState, pNetChannel->iOutReliableState, pNetChannel->iInSequenceNr, I::Globals->flRealTime));
	}

	// is cached too much sequences
	if (vecSequences.size() > 2048U)
		vecSequences.pop_back();
}

void CLagCompensation::ClearIncomingSequences()
{
	if (!vecSequences.empty())
	{
		nLastIncomingSequence = 0;
		vecSequences.clear();
	}
}

void CLagCompensation::AddLatencyToNetChannel(INetChannel* pNetChannel, float flLatency)
{
	for (const auto& sequence : vecSequences)
	{
		if (I::Globals->flRealTime - sequence.flCurrentTime >= flLatency)
		{
			pNetChannel->iInReliableState = sequence.iInReliableState;
			pNetChannel->iInSequenceNr = sequence.iSequenceNr;
			break;
		}
	}
}

void LagRecord::SaveRecord(CBaseEntity* player)
{
	m_vecOrigin = player->GetOrigin();
	m_vecAbsOrigin = player->GetAbsOrigin();
	m_angAngles = player->GetEyeAngles();
	m_flSimulationTime = player->GetSimulationTime();
	m_vecMins = player->GetCollideable()->OBBMins();
	m_vecMax = player->GetCollideable()->OBBMaxs();
	m_nFlags = player->GetFlags();
	m_vecVelocity = player->GetVelocity();

	m_arrflPoseParameters = player->GetPoseParameter();

	m_iTickCount = I::Globals->iTickCount;
	if (player->GetBonePosition(8).has_value())
		m_vecHeadSpot = player->GetBonePosition(8).value();
}

void CMBacktracking::FrameUpdatePostEntityThink()
{
	static auto sv_unlag = I::ConVar->FindVar("sv_unlag");
	if (I::Globals->nMaxClients <= 1 || !sv_unlag->GetBool())
	{
		CMBacktracking::Get().ClearHistory();
		return;
	}

	for (int i = 1; i <= I::Globals->nMaxClients; i++)
	{
		CBaseEntity* player = I::ClientEntityList->Get<CBaseEntity>(i);

		auto& lag_records = this->m_LagRecord[i];

		if (!IsPlayerValid(player))
		{
			if (lag_records.size() > 0)
				lag_records.clear();

			continue;
		}

		int32_t ent_index = player->GetIndex();
		float_t sim_time = player->GetSimulationTime();

		LagRecord cur_lagrecord;

		RemoveBadRecords(ent_index, lag_records);

		if (lag_records.size() > 0)
		{
			auto& tail = lag_records.back();

			if (tail.m_flSimulationTime == sim_time)
				continue;
		}

		UpdateAnimations(player); // update animations
		cur_lagrecord.SaveRecord(player); // first let's create the record

		if (!lag_records.empty()) // apply specific stuff that is needed
		{
			auto& temp_lagrecord = lag_records.back();
			int32_t priority_level = GetPriorityLevel(player, &temp_lagrecord);

			cur_lagrecord.m_iPriority = priority_level;
			cur_lagrecord.m_flPrevLowerBodyYaw = temp_lagrecord.m_flPrevLowerBodyYaw;
			cur_lagrecord.m_arrflPrevPoseParameters = temp_lagrecord.m_arrflPrevPoseParameters;

			if (priority_level == 3)
				cur_lagrecord.m_angAngles.y = temp_lagrecord.m_angAngles.y;
		}

		lag_records.emplace_back(cur_lagrecord);
	}
}

void CMBacktracking::ProcessCMD(int iTargetIdx, CUserCmd* usercmd)
{
	LagRecord recentLR = m_RestoreLagRecord[iTargetIdx].first;
	if (!IsTickValid(TIME_TO_TICKS(recentLR.m_flSimulationTime)))
		
		usercmd->iTickCount = TIME_TO_TICKS(I::ClientEntityList->Get<CBaseEntity>(iTargetIdx)->GetSimulationTime() + GetLerpTime());
	else
		usercmd->iTickCount = TIME_TO_TICKS(recentLR.m_flSimulationTime + GetLerpTime());
}

void CMBacktracking::RemoveBadRecords(int Idx, std::deque<LagRecord>& records)
{
	auto& m_LagRecords = records; // Should use rbegin but can't erase
	for (auto lag_record = m_LagRecords.begin(); lag_record != m_LagRecords.end(); lag_record++)
	{
		if (!IsTickValid(TIME_TO_TICKS(lag_record->m_flSimulationTime)))
		{
			m_LagRecords.erase(lag_record);
			if (!m_LagRecords.empty())
				lag_record = m_LagRecords.begin();
			else break;
		}
	}
}

bool CMBacktracking::StartLagCompensation(CBaseEntity* player)
{
	backtrack_records.clear();

	enum
	{
		// Only try to awall the "best" records, otherwise fail.
		TYPE_BEST_RECORDS,
		// Only try to awall the newest and the absolute best record.
		TYPE_BEST_AND_NEWEST,
		// Awall everything (fps killer)
		TYPE_ALL_RECORDS,
	};

	auto& m_LagRecords = this->m_LagRecord[player->GetIndex()];
	m_RestoreLagRecord[player->GetIndex()].second.SaveRecord(player);

	switch (TYPE_BEST_RECORDS)
	{
	case TYPE_BEST_RECORDS:
	{
		for (auto it : m_LagRecords)
		{
			if (it.m_iPriority >= 1 || (it.m_vecVelocity.Length2D() > 10.f)) // let's account for those moving fags aswell -> it's experimental and not supposed what this lagcomp mode should do
				backtrack_records.emplace_back(it);
		}
		break;
	}
	case TYPE_BEST_AND_NEWEST:
	{
		LagRecord newest_record = LagRecord();
		for (auto it : m_LagRecords)
		{
			if (it.m_flSimulationTime > newest_record.m_flSimulationTime)
				newest_record = it;

			if (it.m_iPriority >= 1 /*&& !(it.m_nFlags & FL_ONGROUND) && it.m_vecVelocity.Length2D() > 150*/)
				backtrack_records.emplace_back(it);
		}
		backtrack_records.emplace_back(newest_record);
		break;
	}
	case TYPE_ALL_RECORDS:
		// Ouch, the fps drop will be H U G E.
		backtrack_records = m_LagRecords;
		break;
	}

	std::sort(backtrack_records.begin(), backtrack_records.end(), [](LagRecord const& a, LagRecord const& b) { return a.m_iPriority > b.m_iPriority; });
	return backtrack_records.size() > 0;
}

bool CMBacktracking::FindViableRecord(CBaseEntity* player, LagRecord* record)
{
	auto& m_LagRecords = this->m_LagRecord[player->GetIndex()];

	// Ran out of records to check. Go back.
	if (backtrack_records.empty())
	{
		return false;
	}

	LagRecord
		recentLR = *backtrack_records.begin(),
		prevLR;

	// Should still use m_LagRecords because we're checking for LC break.
	auto iter = std::find(m_LagRecords.begin(), m_LagRecords.end(), recentLR);
	auto idx = std::distance(m_LagRecords.begin(), iter);
	if (0 != idx) prevLR = *std::prev(iter);

	// Saving first record for processcmd.
	m_RestoreLagRecord[player->GetIndex()].first = recentLR;

	if (!IsTickValid(TIME_TO_TICKS(recentLR.m_flSimulationTime)))
	{
		backtrack_records.pop_front();
		return backtrack_records.size() > 0; // RET_NO_RECORDS true false
	}

	// Remove a record...
	backtrack_records.pop_front();

	if ((0 != idx) && (recentLR.m_vecOrigin - prevLR.m_vecOrigin).LengthSqr() > 4096.f)
	{
		/*float simulationTimeDelta = recentLR.m_flSimulationTime - prevLR.m_flSimulationTime;

		int simulationTickDelta = clamp(TIME_TO_TICKS(simulationTimeDelta), 1, 15);

		for (; simulationTickDelta > 0; simulationTickDelta--)
			RebuildGameMovement::Get().FullWalkMove(player);*/
		FakelagFix(player);

		// Bandage fix so we "restore" to the lagfixed player.
		m_RestoreLagRecord[player->GetIndex()].second.SaveRecord(player);
		*record = m_RestoreLagRecord[player->GetIndex()].second;

		// Clear so we don't try to bt shit we can't
		backtrack_records.clear();

		return true; // Return true so we still try to aimbot.
	}
	else
	{

		player->GetCollideable()->OBBMins() = recentLR.m_vecMins;
		player->GetCollideable()->OBBMaxs() = recentLR.m_vecMax;

		player->SetAbsAngles(QAngle(0, recentLR.m_angAngles.y, 0));
		player->SetAbsOrigin(recentLR.m_vecOrigin);

		player->GetFlags() = recentLR.m_nFlags;

		player->GetPoseParameter() = recentLR.m_arrflPoseParameters;

		*record = recentLR;
		return true;
	}
}

void CMBacktracking::FinishLagCompensation(CBaseEntity* player)
{
	int idx = player->GetIndex();

	//player->GetCollideable()->OBBMins() = m_RestoreLagRecord[idx].second.m_vecMins;
	//player->GetCollideable()->OBBMaxs() = m_RestoreLagRecord[idx].second.m_vecMax;

	player->SetAbsAngles(QAngle(0, m_RestoreLagRecord[idx].second.m_angAngles.y, 0));
	player->SetAbsOrigin(m_RestoreLagRecord[idx].second.m_vecOrigin);

	player->GetFlags() = m_RestoreLagRecord[idx].second.m_nFlags;

	player->GetPoseParameter() = m_RestoreLagRecord[idx].second.m_arrflPoseParameters;
}

int CMBacktracking::GetPriorityLevel(CBaseEntity* player, LagRecord* lag_record)
{
	int priority = 0;

	if (lag_record->m_flPrevLowerBodyYaw != player->GetLowerBodyYaw())
	{
		lag_record->m_angAngles.y = player->GetLowerBodyYaw();
		priority = 3;
	}

	if ((player->GetPoseParameter()[1] > (0.85f) && lag_record->m_arrflPrevPoseParameters[1] <= (0.85f)) || (player->GetPoseParameter()[1] <= (0.85f) && lag_record->m_arrflPrevPoseParameters[1] > (0.85f)))
		priority = 1;

	lag_record->m_flPrevLowerBodyYaw = player->GetLowerBodyYaw();
	lag_record->m_arrflPrevPoseParameters = player->GetPoseParameter();

	return priority;
}

void CMBacktracking::SimulateMovement(Vector& velocity, Vector& origin, CBaseEntity* player, int& flags, bool was_in_air)
{
	if (!(flags & FL_ONGROUND))
		velocity.z -= (I::Globals->flFrameTime * I::ConVar->FindVar("sv_gravity")->GetFloat());
	else if (was_in_air)
		velocity.z = I::ConVar->FindVar("sv_jump_impulse")->GetFloat();

	const Vector mins = player->GetCollideable()->OBBMins();
	const Vector max = player->GetCollideable()->OBBMaxs();

	const Vector src = origin;
	Vector end = src + (velocity * I::Globals->flFrameTime);

	Trace_t trace;
	CTraceFilter filter = CTraceFilter(player);

	I::EngineTrace->TraceRay(Ray_t(src, end, mins, max), MASK_PLAYERSOLID, &filter, &trace);

	if (trace.flFraction != 1.f)
	{
		for (int i = 0; i < 2; i++)
		{
			velocity -= trace.plane.vecNormal * velocity.DotProduct(trace.plane.vecNormal);

			const float dot = velocity.DotProduct(trace.plane.vecNormal);
			if (dot < 0.f)
			{
				velocity.x -= dot * trace.plane.vecNormal.x;
				velocity.y -= dot * trace.plane.vecNormal.y;
				velocity.z -= dot * trace.plane.vecNormal.z;
			}

			end = trace.vecEnd + (velocity * (I::Globals->flIntervalPerTick * (1.f - trace.flFraction)));

			I::EngineTrace->TraceRay(Ray_t(trace.vecEnd, end, mins, max), MASK_PLAYERSOLID, &filter, &trace);

			if (trace.flFraction == 1.f)
				break;
		}
	}

	origin = trace.vecEnd;
	end = trace.vecEnd;
	end.z -= 2.f;

	I::EngineTrace->TraceRay(Ray_t(origin, end, mins, max), MASK_PLAYERSOLID, &filter, &trace);

	flags &= ~(1 << 0);

	if (trace.DidHit() && trace.plane.vecNormal.z > 0.7f)
		flags |= (1 << 0);
}

void CMBacktracking::FakelagFix(CBaseEntity* player)
{
	auto& lag_records = this->m_LagRecord[player->GetIndex()];

	if (lag_records.size() > 2)
	{
		bool bChocked = TIME_TO_TICKS(player->GetSimulationTime() - lag_records.back().m_flSimulationTime) > 1;
		bool bInAir = false;

		if (!(player->GetFlags() & FL_ONGROUND) || !(lag_records.back().m_nFlags & FL_ONGROUND))
			bInAir = true;

		if (bChocked)
		{
			player->GetOrigin() = lag_records.back().m_vecOrigin;
			player->SetAbsOrigin(lag_records.back().m_vecAbsOrigin);
			player->GetVelocity() = lag_records.back().m_vecVelocity;
			player->GetFlags() = lag_records.back().m_nFlags;
		}

		Vector data_origin = player->GetOrigin();
		Vector data_velocity = player->GetVelocity();
		int data_flags = player->GetFlags();

		if (bChocked)
		{
			SimulateMovement(data_velocity, data_origin, player, data_flags, bInAir);

			player->GetOrigin() = data_origin;
			player->SetAbsOrigin(data_origin);
			player->GetVelocity() = data_velocity;

			player->GetFlags() &= 0xFFFFFFFE;
			auto penultimate_record = *std::prev(lag_records.end(), 2);
			if ((lag_records.back().m_nFlags & FL_ONGROUND) && (penultimate_record.m_nFlags & FL_ONGROUND))
				player->GetFlags() |= 1;
			if (*(float*)((uintptr_t)player->GetAnimationLayer(0) + 0x138) > 0.f)
				player->GetFlags() |= 1;
		}
	}
}

void CMBacktracking::UpdateAnimations(CBaseEntity* player)
{
	CCSGOPlayerAnimState* state = player->GetAnimationState();
	if (state)
	{
		// backup
		const float curtime = I::Globals->flCurrentTime;
		const float frametime = I::Globals->flFrameTime;

		static auto host_timescale = I::ConVar->FindVar(("host_timescale"));

		I::Globals->flFrameTime = I::Globals->flIntervalPerTick * host_timescale->GetFloat();
		I::Globals->flCurrentTime = player->GetOldSimulationTime() + I::Globals->flIntervalPerTick;

		Vector backup_origin = player->GetOrigin();
		Vector backup_absorigin = player->GetAbsOrigin();
		Vector backup_velocity = player->GetVelocity();
		int backup_flags = player->GetFlags();
		int backup_eflags = player->GetEFlags();

		CAnimationLayer backup_layers[15];
		if (state->bOnGround)
		{
			player->GetFlags() |= FL_ONGROUND;
		}
		else
		{
			player->GetFlags() &= ~FL_ONGROUND;
		}
		player->GetEFlags() &= ~0x1000;

		player->GetAbsVelocity() = player->GetVelocity();

		// invalidates prior animations so the entity gets animated on our client 100% via UpdateClientSideAnimation
		if (state->iLastUpdateFrame == I::Globals->iFrameCount)
			state->iLastUpdateFrame = I::Globals->iFrameCount - 1;

		player->IsClientSideAnimation() = true;

		// updates local animations + poses + calculates new abs angle based on eyeangles and other stuff
		player->UpdateClientSideAnimations();

		player->IsClientSideAnimation() = false;

		player->GetVelocity() = backup_velocity;
		player->GetFlags() = backup_flags;
		player->GetEFlags() = backup_eflags;

		I::Globals->flCurrentTime = curtime;
		I::Globals->flFrameTime = frametime;

		player->SetupBones(nullptr, -1, 0x7FF00, I::Globals->flCurrentTime);
	}
}

void CMBacktracking::SetOverwriteTick(CBaseEntity* player, QAngle angles, float_t correct_time, uint32_t priority)
{
	int idx = player->GetIndex();
	LagRecord overwrite_record;
	auto& m_LagRecords = this->m_LagRecord[player->GetIndex()];

	if (!IsTickValid(TIME_TO_TICKS(correct_time)))
		I::ConVar->ConsoleColorPrintf(Color(255, 0, 0, 255), "Dev Error: failed to overwrite tick, delta too big. Priority: %d\n", priority);

	overwrite_record.SaveRecord(player);
	overwrite_record.m_angAngles = angles;
	overwrite_record.m_iPriority = priority;
	overwrite_record.m_flSimulationTime = correct_time;
	m_LagRecords.emplace_back(overwrite_record);
}

void CMBacktracking::RageBacktrack(CBaseEntity* target, CUserCmd* usercmd, Vector& aim_point, bool& hitchanced)
{
	auto firedShots ();
	if (StartLagCompensation(target))
	{
		LagRecord cur_record;
		auto& m_LagRecords = this->m_LagRecord[target->GetIndex()];
		while (FindViableRecord(target, &cur_record))
		{
			auto iter = std::find(m_LagRecords.begin(), m_LagRecords.end(), cur_record);
			if (iter == m_LagRecords.end())
				continue;

			if (iter->m_bNoGoodSpots)
			{
				// Already awalled from same spot, don't try again like a dumbass.
				if (iter->m_vecLocalAimspot == G::pLocal->GetEyePosition())
					continue;
				else
					iter->m_bNoGoodSpots = false;
			}

			if (!iter->m_bMatrixBuilt)
			{
				if (!target->SetupBones(iter->matrix, 128, 256, iter->m_flSimulationTime))
					continue;

				iter->m_bMatrixBuilt = true;
			}

			aim_point = CRageBot::Get().CalculateBestPoint(target, HITBOX_PELVIS, C::Get<float>(Vars.fRageMinDamage), true, iter->matrix);
			if (!aim_point.IsValid())
			{
				FinishLagCompensation(target);
				iter->m_bNoGoodSpots = true;
				iter->m_vecLocalAimspot = G::pLocal->GetEyePosition();
				continue;
			}

			//QAngle aimAngle = M::CalcAngle(G::pLocal->GetEyePosition(), aim_point) - (g_Options.rage_norecoil ? G::pLocal->GetViewPunch() * 2.f : QAngle(0, 0, 0));

			//hitchanced = AimRage::Get().HitChance(aimAngle, target, g_Options.rage_hitchance_amount);

			//this->current_record[target->EntIndex()] = *iter;
			break;
		}
		FinishLagCompensation(target);
		ProcessCMD(target->GetIndex(), usercmd);
	}
}

bool CMBacktracking::IsTickValid(int tick)
{
	// better use polak's version than our old one, getting more accurate results

	INetChannelInfo* nci = I::Engine->GetNetChannelInfo();

	static auto sv_maxunlag = I::ConVar->FindVar("sv_maxunlag");

	if (!nci || !sv_maxunlag)
		return false;

	float correct = clamp(nci->GetLatency(FLOW_OUTGOING) + GetLerpTime(), 0.f, sv_maxunlag->GetFloat());

	float deltaTime = correct - (I::Globals->flCurrentTime - TICKS_TO_TIME(tick));

	return fabsf(deltaTime) < 0.2f;
}

bool CMBacktracking::IsPlayerValid(CBaseEntity* player)
{
	if (!player)
		return false;

	if (!player->IsPlayer())
		return false;

	if (player == G::pLocal)
		return false;

	if (player->IsDormant())
		return false;

	if (!player->IsAlive())
		return false;

	if (player->GetTeam() == G::pLocal->GetTeam())
		return false;

	if (player->HasImmunity())
		return false;

	return true;
}

float CMBacktracking::GetLerpTime()
{
	int ud_rate = I::ConVar->FindVar("cl_updaterate")->GetInt();
	CConVar* min_ud_rate = I::ConVar->FindVar("sv_minupdaterate");
	CConVar* max_ud_rate = I::ConVar->FindVar("sv_maxupdaterate");

	if (min_ud_rate && max_ud_rate)
		ud_rate = max_ud_rate->GetInt();

	float ratio = I::ConVar->FindVar("cl_interp_ratio")->GetFloat();

	if (ratio == 0)
		ratio = 1.0f;

	float lerp = I::ConVar->FindVar("cl_interp")->GetFloat();
	CConVar* c_min_ratio = I::ConVar->FindVar("sv_client_min_interp_ratio");
	CConVar* c_max_ratio = I::ConVar->FindVar("sv_client_max_interp_ratio");

	if (c_min_ratio && c_max_ratio && c_min_ratio->GetFloat() != 1)
		ratio = clamp(ratio, c_min_ratio->GetFloat(), c_max_ratio->GetFloat());

	return max2(lerp, (ratio / ud_rate));
}

template<class T, class U>
T CMBacktracking::clamp(T in, U low, U high)
{
	if (in <= low)
		return low;

	if (in >= high)
		return high;

	return in;
}