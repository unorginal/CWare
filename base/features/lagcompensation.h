#pragma once
// used: std::deque
#include <deque>

// used: winapi definitions
#include "../common.h"
// used: usercmd
#include "../sdk/datatypes/usercmd.h"
// used: netchannel
// used: globals interface
#include "../core/interfaces.h"
#include "../sdk/interfaces/inetchannel.h"
#include "../global.h"
#include "../features/ragebot.h"
#include "../core/variables.h"
#define max2(a,b)            (((a) > (b)) ? (a) : (b))
#pragma region lagcompensation_definitions
#define LAG_COMPENSATION_TELEPORTED_DISTANCE_SQR ( 64.0f * 64.0f )
#define LAG_COMPENSATION_EPS_SQR ( 0.1f * 0.1f )
#define LAG_COMPENSATION_ERROR_EPS_SQR ( 4.0f * 4.0f )
#pragma endregion

#pragma region lagcompensation_enumerations
enum ELagCompensationState
{
	LC_NO =					0,
	LC_ALIVE =				(1 << 0),
	LC_ORIGIN_CHANGED =		(1 << 8),
	LC_ANGLES_CHANGED =		(1 << 9),
	LC_SIZE_CHANGED =		(1 << 10),
	LC_ANIMATION_CHANGED =	(1 << 11)
};
#pragma endregion

struct SequenceObject_t
{
	SequenceObject_t(int iInReliableState, int iOutReliableState, int iSequenceNr, float flCurrentTime)
		: iInReliableState(iInReliableState), iOutReliableState(iOutReliableState), iSequenceNr(iSequenceNr), flCurrentTime(flCurrentTime) { }

	int iInReliableState;
	int iOutReliableState;
	int iSequenceNr;
	float flCurrentTime;
};

// @note: FYI - https://www.unknowncheats.me/forum/counterstrike-global-offensive/280912-road-perfect-aimbot-1-interpolation.html


struct LayerRecord
{
	LayerRecord()
	{
		m_nOrder = 0;
		m_nSequence = 0;
		m_flWeight = 0.f;
		m_flCycle = 0.f;
	}

	LayerRecord(const LayerRecord& src)
	{
		m_nOrder = src.m_nOrder;
		m_nSequence = src.m_nSequence;
		m_flWeight = src.m_flWeight;
		m_flCycle = src.m_flCycle;
	}

	uint32_t m_nOrder;
	uint32_t m_nSequence;
	float_t m_flWeight;
	float_t m_flCycle;
};

struct LagRecord
{
	LagRecord()
	{
		m_iPriority = -1;

		m_flSimulationTime = -1.f;
		m_vecOrigin = Vector();
		m_angAngles = QAngle();
		m_vecMins = Vector();
		m_vecMax = Vector();
		m_bMatrixBuilt = false;
	}

	bool operator==(const LagRecord& rec)
	{
		return m_flSimulationTime == rec.m_flSimulationTime;
	}

	void SaveRecord(CBaseEntity* player);

	matrix3x4_t	matrix[128];
	bool m_bMatrixBuilt;
	Vector m_vecHeadSpot;
	float m_iTickCount;

	// For priority/other checks
	int32_t m_iPriority;
	Vector  m_vecVelocity;
	float_t m_flPrevLowerBodyYaw;
	std::array<float_t, 24> m_arrflPrevPoseParameters;
	Vector  m_vecLocalAimspot;
	bool    m_bNoGoodSpots;

	// For backtracking
	float_t m_flSimulationTime;
	int32_t m_nFlags;
	Vector m_vecOrigin;	   // Server data, change to this for accuracy
	Vector m_vecAbsOrigin;
	QAngle m_angAngles;
	Vector m_vecMins;
	Vector m_vecMax;
	std::array<float_t, 24> m_arrflPoseParameters;
	std::array<LayerRecord, 15> m_LayerRecords;
};

class CLagCompensation : public CSingleton<CLagCompensation>
{
public:
	// Get
	void Run(CUserCmd* pCmd);

	// Main
	void UpdateIncomingSequences(INetChannel* pNetChannel);
	void ClearIncomingSequences();
	void AddLatencyToNetChannel(INetChannel* pNetChannel, float flLatency);

private:
	// Values
	std::deque<SequenceObject_t> vecSequences = { };
	/* our real incoming sequences count */
	int nRealIncomingSequence = 0;
	/* count of incoming sequences what we can spike */
	int nLastIncomingSequence = 0;
};

class CMBacktracking : public CSingleton<CMBacktracking>
{
public:

	void RageBacktrack(CBaseEntity* target, CUserCmd* usercmd, Vector& aim_point, bool& hitchanced);

	std::deque<LagRecord> m_LagRecord[64]; // All records
	std::pair<LagRecord, LagRecord> m_RestoreLagRecord[64]; // Used to restore/change
	std::deque<LagRecord> backtrack_records; // Needed to select records based on menu option.
	LagRecord current_record[64]; // Used for various other actions where we need data of the current lag compensated player

	inline void ClearHistory()
	{
		for (int i = 0; i < 64; i++)
			m_LagRecord[i].clear();
	}

	void FrameUpdatePostEntityThink();
	void ProcessCMD(int iTargetIdx, CUserCmd* usercmd);

	void RemoveBadRecords(int Idx, std::deque<LagRecord>& records);

	bool StartLagCompensation(CBaseEntity* player);
	bool FindViableRecord(CBaseEntity* player, LagRecord* record);
	void FinishLagCompensation(CBaseEntity* player);

	int GetPriorityLevel(CBaseEntity* player, LagRecord* lag_record);
	void SimulateMovement(Vector& velocity, Vector& origin, CBaseEntity* player, int& flags, bool was_in_air);
	void FakelagFix(CBaseEntity* player);
	void UpdateAnimations(CBaseEntity* player);

	void SetOverwriteTick(CBaseEntity* player, QAngle angles, float_t correct_time, uint32_t priority);

	bool IsTickValid(int tick);
	bool IsPlayerValid(CBaseEntity* player);
	float GetLerpTime();

	template<class T, class U>
	T clamp(T in, U low, U high);
};