#include "math.h"

// used: modules definitons, convar, globals interfaces
#include "../core/interfaces.h"

bool M::Setup()
{
	const void* hVstdLib = MEM::GetModuleBaseHandle(VSTDLIB_DLL);

	if (hVstdLib == nullptr)
		return false;

	RandomSeed = reinterpret_cast<RandomSeedFn>(MEM::GetExportAddress(hVstdLib, XorStr("RandomSeed")));
	if (RandomSeed == nullptr)
		return false;

	RandomFloat = reinterpret_cast<RandomFloatFn>(MEM::GetExportAddress(hVstdLib, XorStr("RandomFloat")));
	if (RandomFloat == nullptr)
		return false;

	RandomFloatExp = reinterpret_cast<RandomFloatExpFn>(MEM::GetExportAddress(hVstdLib, XorStr("RandomFloatExp")));
	if (RandomFloatExp == nullptr)
		return false;

	RandomInt = reinterpret_cast<RandomIntFn>(MEM::GetExportAddress(hVstdLib, XorStr("RandomInt")));
	if (RandomInt == nullptr)
		return false;

	RandomGaussianFloat = reinterpret_cast<RandomGaussianFloatFn>(MEM::GetExportAddress(hVstdLib, XorStr("RandomGaussianFloat")));
	if (RandomGaussianFloat == nullptr)
		return false;

	return true;
}

float M::RandomFloatFunc(float min, float max)
{
	static auto ranFloat = reinterpret_cast<float(*)(float, float)>(GetProcAddress(GetModuleHandleW(L"vstdlib.dll"), "RandomFloat"));
	if (ranFloat)
	{
		return ranFloat(min, max);
	}
	else
	{
		return 0.f;
	}
}
float M::NormalizeYaw(float angle) {
	if (!std::isfinite(angle))
		angle = 0.f;

	return std::remainderf(angle, 360.0f);
}
void M::fast_rsqrt(float a, float* out)
{
	const auto xx = _mm_load_ss(&a);
	auto xr = _mm_rsqrt_ss(xx);
	auto xt = _mm_mul_ss(xr, xr);
	xt = _mm_mul_ss(xt, xx);
	xt = _mm_sub_ss(_mm_set_ss(3.f), xt);
	xt = _mm_mul_ss(xt, _mm_set_ss(0.5f));
	xr = _mm_mul_ss(xr, xt);
	_mm_store_ss(out, xr);
}
float M::fast_vec_normalize(Vector& vec)
{
	const auto sqrlen = vec.LengthSqr() + 1.0e-10f;
	float invlen;
	fast_rsqrt(sqrlen, &invlen);
	vec.x *= invlen;
	vec.y *= invlen;
	vec.z *= invlen;
	return sqrlen * invlen;
}
Vector M::CrossProduct2(const Vector& a, const Vector& b)
{
	return Vector(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

void M::VectorAngles1337(const Vector& forward, Vector& up, QAngle& angles)
{
	Vector left = CrossProduct2(up, forward);
	left.NormalizeInPlace();

	float forwardDist = forward.Length2D();

	if (forwardDist > 0.001f)
	{
		angles.x = atan2f(-forward.z, forwardDist) * 180 / M_PI;
		angles.y = atan2f(forward.y, forward.x) * 180 / M_PI;

		float upZ = (left.y * forward.x) - (left.x * forward.y);
		angles.z = atan2f(left.z, upZ) * 180 / M_PI;
	}
	else
	{
		angles.x = atan2f(-forward.z, forwardDist) * 180 / M_PI;
		angles.y = atan2f(-left.x, left.y) * 180 / M_PI;
		angles.z = 0;
	}
}
void M::VectorAngles(const Vector& vecForward, QAngle& angView)
{
	float flPitch, flYaw;

	if (vecForward.x == 0.f && vecForward.y == 0.f)
	{
		flPitch = (vecForward.z > 0.f) ? 270.f : 90.f;
		flYaw = 0.f;
	}
	else
	{
		flPitch = std::atan2f(-vecForward.z, vecForward.Length2D()) * 180.f / M_PI;

		if (flPitch < 0.f)
			flPitch += 360.f;

		flYaw = std::atan2f(vecForward.y, vecForward.x) * 180.f / M_PI;

		if (flYaw < 0.f)
			flYaw += 360.f;
	}

	angView.x = flPitch;
	angView.y = flYaw;
	angView.z = 0.f;
}

void M::AngleVectors(const QAngle& angView, Vector* pForward, Vector* pRight, Vector* pUp)
{
	float sp, sy, sr, cp, cy, cr;

	DirectX::XMScalarSinCos(&sp, &cp, M_DEG2RAD(angView.x));
	DirectX::XMScalarSinCos(&sy, &cy, M_DEG2RAD(angView.y));
	DirectX::XMScalarSinCos(&sr, &cr, M_DEG2RAD(angView.z));

	if (pForward != nullptr)
	{
		pForward->x = cp * cy;
		pForward->y = cp * sy;
		pForward->z = -sp;
	}

	if (pRight != nullptr)
	{
		pRight->x = -1 * sr * sp * cy + -1 * cr * -sy;
		pRight->y = -1 * sr * sp * sy + -1 * cr * cy;
		pRight->z = -1 * sr * cp;
	}

	if (pUp != nullptr)
	{
		pUp->x = cr * sp * cy + -sr * -sy;
		pUp->y = cr * sp * sy + -sr * cy;
		pUp->z = cr * cp;
	}
}

void M::AngleMatrix(const QAngle& angView, matrix3x4_t& matOutput, const Vector& vecOrigin)
{
	float sp, sy, sr, cp, cy, cr;

	DirectX::XMScalarSinCos(&sp, &cp, M_DEG2RAD(angView.x));
	DirectX::XMScalarSinCos(&sy, &cy, M_DEG2RAD(angView.y));
	DirectX::XMScalarSinCos(&sr, &cr, M_DEG2RAD(angView.z));

	matOutput.SetForward(Vector(cp * cy, cp * sy, -sp));

	const float crcy = cr * cy;
	const float crsy = cr * sy;
	const float srcy = sr * cy;
	const float srsy = sr * sy;

	matOutput.SetLeft(Vector(sp * srcy - crsy, sp * srsy + crcy, sr * cp));
	matOutput.SetUp(Vector(sp * crcy + srsy, sp * crsy - srcy, cr * cp));
	matOutput.SetOrigin(vecOrigin);
}

Vector2D M::AnglePixels(const float flSensitivity, const float flPitch, const float flYaw, const QAngle& angBegin, const QAngle& angEnd)
{
	QAngle angDelta = angBegin - angEnd;
	angDelta.Normalize();

	const float flPixelMovePitch = (-angDelta.x) / (flYaw * flSensitivity);
	const float flPixelMoveYaw = (angDelta.y) / (flPitch * flSensitivity);

	return Vector2D(flPixelMoveYaw, flPixelMovePitch);
}



QAngle M::PixelsAngle(const float flSensitivity, const float flPitch, const float flYaw, const Vector2D& vecPixels)
{
	const float flAngleMovePitch = (-vecPixels.x) * (flYaw * flSensitivity);
	const float flAngleMoveYaw = (vecPixels.y) * (flPitch * flSensitivity);

	return QAngle(flAngleMoveYaw, flAngleMovePitch, 0.f);
}

QAngle M::CalcAngle(const Vector& vecStart, const Vector& vecEnd)
{
	QAngle angView;
	const Vector vecDelta = vecEnd - vecStart;
	VectorAngles(vecDelta, angView);
	angView.Normalize();

	return angView;
}

Vector M::VectorTransform(const Vector& vecTransform, const matrix3x4_t& matrix)
{
	return Vector(vecTransform.DotProduct(matrix[0]) + matrix[0][3],
		vecTransform.DotProduct(matrix[1]) + matrix[1][3],
		vecTransform.DotProduct(matrix[2]) + matrix[2][3]);
}

Vector M::ExtrapolateTick(const Vector& p0, const Vector& v0)
{
	// position formula: p0 + v0t
	return p0 + (v0 * I::Globals->flIntervalPerTick);
}

void M::RotatePoint(const ImVec2& vecIn, const float flAngle, ImVec2* pOutPoint)
{
	if (&vecIn == pOutPoint)
	{
		const ImVec2 vecPoint = vecIn;
		RotatePoint(vecPoint, flAngle, pOutPoint);
		return;
	}

	const float flSin = std::sinf(M_DEG2RAD(flAngle));
	const float flCos = std::cosf(M_DEG2RAD(flAngle));

	pOutPoint->x = vecIn.x * flCos - vecIn.y * flSin;
	pOutPoint->y = vecIn.x * flSin + vecIn.y * flCos;
}

void M::RotateCenter(const ImVec2& vecCenter, const float flAngle, ImVec2* pOutPoint)
{
	const float flSin = std::sinf(M_DEG2RAD(flAngle));
	const float flCos = std::cosf(M_DEG2RAD(flAngle));

	pOutPoint->x -= vecCenter.x;
	pOutPoint->y -= vecCenter.y;

	const float x = pOutPoint->x * flCos - pOutPoint->y * flSin;
	const float y = pOutPoint->x * flSin + pOutPoint->y * flCos;

	pOutPoint->x = x + vecCenter.x;
	pOutPoint->y = y + vecCenter.y;
}


void GM::AngleVectors(const QAngle& angles, Vector& forward)
{
	float	sp, sy, cp, cy;

	SinCos(M_DEG2RAD(angles.x), &sy, &cy);
	SinCos(M_DEG2RAD(angles.y), &sp, &cp);

	forward.x = cp * cy;
	forward.y = cp * sy;
	forward.z = -sp;
}
void GM::SinCos(float a, float* s, float* c)
{
	*s = sin(a);
	*c = cos(a);
}

void GM::VectorTransform(const Vector& in1, const matrix3x4_t& in2, Vector& out)
{
	out[0] = in1.DotProduct(in2[0]) + in2[0][3];
	out[1] = in1.DotProduct(in2[1]) + in2[1][3];
	out[2] = in1.DotProduct(in2[2]) + in2[2][3];
}

void GM::vector_transform(const Vector& in1, const matrix3x4_t& in2, Vector& out)
{
	out = Vector(in1.DotProduct(Vector(in2[0][0], in2[0][1], in2[0][2])) + in2[0][3], in1.DotProduct(Vector(in2[1][0], in2[1][1], in2[1][2])) + in2[1][3], in1.DotProduct(Vector(in2[2][0], in2[2][1], in2[2][2])) + in2[2][3]);
}

bool GM::screen_transform(const Vector& in, Vector& out)
{
	static ViewMatrix_t w2sMatrix = I::Engine->WorldToScreenMatrix();

	out.x = w2sMatrix[0][0] * in.x + w2sMatrix[0][1] * in.y + w2sMatrix[0][2] * in.z + w2sMatrix[0][3];
	out.y = w2sMatrix[1][0] * in.x + w2sMatrix[1][1] * in.y + w2sMatrix[1][2] * in.z + w2sMatrix[1][3];
	out.z = 0.0f;

	float w = w2sMatrix[3][0] * in.x + w2sMatrix[3][1] * in.y + w2sMatrix[3][2] * in.z + w2sMatrix[3][3];

	if (w < 0.001f) {
		out.x *= 100000;
		out.y *= 100000;
		return false;
	}

	out.x /= w;
	out.y /= w;

	return true;
}
//--------------------------------------------------------------------------------
bool GM::WorldToScreen(const Vector& in, Vector& out)
{
	if (screen_transform(in, out)) {
		int w, h;
		I::Engine->GetScreenSize(w, h);

		out.x = (w / 2.0f) + (out.x * w) / 2.0f;
		out.y = (h / 2.0f) - (out.y * h) / 2.0f;

		return true;
	}
	return false;
}