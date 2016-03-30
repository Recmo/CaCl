// Based on the Keccak implementation Bertoni et al. which is public domain
//
// Changes made to the original version:
// - Reformatted as C++ source code
// - Choice between 12 and 24 rounds
// - Note: intermediate values might be spilled to the stack and not cleared on
//   return
//
#include "KeccakF1600.h"
using std::uint64_t;

const uint64_t KeccakF1600::_roundConstants[24] = {
	0x0000000000000001ULL, 0x0000000000008082ULL, 0x800000000000808AULL,
	0x8000000080008000ULL, 0x000000000000808BULL, 0x0000000080000001ULL,
	0x8000000080008081ULL, 0x8000000000008009ULL, 0x000000000000008AULL,
	0x0000000000000088ULL, 0x0000000080008009ULL, 0x000000008000000AULL,
	0x000000008000808BULL, 0x800000000000008BULL, 0x8000000000008089ULL,
	0x8000000000008003ULL, 0x8000000000008002ULL, 0x8000000000000080ULL,
	0x000000000000800AULL, 0x800000008000000AULL, 0x8000000080008081ULL,
	0x8000000000008080ULL, 0x0000000080000001ULL, 0x8000000080008008ULL
};

#define rotate_left(a, offset) \
	((static_cast<uint64_t>(a) << offset) ^ (static_cast<uint64_t>(a) >> (64 - offset)))

#define thetaRhoPiChiIotaPrepareTheta(i, A, E) \
	Da = Cu^rotate_left(Ce, 1); \
	De = Ca^rotate_left(Ci, 1); \
	Di = Ce^rotate_left(Co, 1); \
	Do = Ci^rotate_left(Cu, 1); \
	Du = Co^rotate_left(Ca, 1); \
\
	A##ba ^= Da; \
	Bba = A##ba; \
	A##ge ^= De; \
	Bbe = rotate_left(A##ge, 44); \
	A##ki ^= Di; \
	Bbi = rotate_left(A##ki, 43); \
	A##mo ^= Do; \
	Bbo = rotate_left(A##mo, 21); \
	A##su ^= Du; \
	Bbu = rotate_left(A##su, 14); \
	E##ba =   Bba ^ (Bbe | Bbi); \
	E##ba ^= _roundConstants[i]; \
	Ca = E##ba; \
	E##be =   Bbe ^ ((~Bbi) | Bbo); \
	Ce = E##be; \
	E##bi =   Bbi ^ (Bbo & Bbu); \
	Ci = E##bi; \
	E##bo =   Bbo ^ (Bbu | Bba); \
	Co = E##bo; \
	E##bu =   Bbu ^ (Bba & Bbe); \
	Cu = E##bu; \
\
	A##bo ^= Do; \
	Bga = rotate_left(A##bo, 28); \
	A##gu ^= Du; \
	Bge = rotate_left(A##gu, 20); \
	A##ka ^= Da; \
	Bgi = rotate_left(A##ka, 3); \
	A##me ^= De; \
	Bgo = rotate_left(A##me, 45); \
	A##si ^= Di; \
	Bgu = rotate_left(A##si, 61); \
	E##ga =   Bga ^ (Bge | Bgi); \
	Ca ^= E##ga; \
	E##ge =   Bge ^ (Bgi & Bgo); \
	Ce ^= E##ge; \
	E##gi =   Bgi ^ (Bgo | (~Bgu)); \
	Ci ^= E##gi; \
	E##go =   Bgo ^ (Bgu | Bga); \
	Co ^= E##go; \
	E##gu =   Bgu ^ (Bga & Bge); \
	Cu ^= E##gu; \
\
	A##be ^= De; \
	Bka = rotate_left(A##be, 1); \
	A##gi ^= Di; \
	Bke = rotate_left(A##gi, 6); \
	A##ko ^= Do; \
	Bki = rotate_left(A##ko, 25); \
	A##mu ^= Du; \
	Bko = rotate_left(A##mu, 8); \
	A##sa ^= Da; \
	Bku = rotate_left(A##sa, 18); \
	E##ka =   Bka ^ (Bke | Bki); \
	Ca ^= E##ka; \
	E##ke =   Bke ^ (Bki & Bko); \
	Ce ^= E##ke; \
	E##ki =   Bki ^ ((~Bko) & Bku); \
	Ci ^= E##ki; \
	E##ko = (~Bko)^ (Bku | Bka); \
	Co ^= E##ko; \
	E##ku =   Bku ^ (Bka & Bke); \
	Cu ^= E##ku; \
\
	A##bu ^= Du; \
	Bma = rotate_left(A##bu, 27); \
	A##ga ^= Da; \
	Bme = rotate_left(A##ga, 36); \
	A##ke ^= De; \
	Bmi = rotate_left(A##ke, 10); \
	A##mi ^= Di; \
	Bmo = rotate_left(A##mi, 15); \
	A##so ^= Do; \
	Bmu = rotate_left(A##so, 56); \
	E##ma =   Bma ^ (Bme & Bmi); \
	Ca ^= E##ma; \
	E##me =   Bme ^ (Bmi | Bmo); \
	Ce ^= E##me; \
	E##mi =   Bmi ^ ((~Bmo) | Bmu); \
	Ci ^= E##mi; \
	E##mo = (~Bmo)^ (Bmu & Bma); \
	Co ^= E##mo; \
	E##mu =   Bmu ^ (Bma | Bme); \
	Cu ^= E##mu; \
\
	A##bi ^= Di; \
	Bsa = rotate_left(A##bi, 62); \
	A##go ^= Do; \
	Bse = rotate_left(A##go, 55); \
	A##ku ^= Du; \
	Bsi = rotate_left(A##ku, 39); \
	A##ma ^= Da; \
	Bso = rotate_left(A##ma, 41); \
	A##se ^= De; \
	Bsu = rotate_left(A##se, 2); \
	E##sa =   Bsa ^ ((~Bse) & Bsi); \
	Ca ^= E##sa; \
	E##se = (~Bse)^ (Bsi | Bso); \
	Ce ^= E##se; \
	E##si =   Bsi ^ (Bso & Bsu); \
	Ci ^= E##si; \
	E##so =   Bso ^ (Bsu | Bsa); \
	Co ^= E##so; \
	E##su =   Bsu ^ (Bsa & Bse); \
	Cu ^= E##su;

#define thetaRhoPiChiIota(i, A, E) \
	Da = Cu^rotate_left(Ce, 1); \
	De = Ca^rotate_left(Ci, 1); \
	Di = Ce^rotate_left(Co, 1); \
	Do = Ci^rotate_left(Cu, 1); \
	Du = Co^rotate_left(Ca, 1); \
\
	A##ba ^= Da; \
	Bba = A##ba; \
	A##ge ^= De; \
	Bbe = rotate_left(A##ge, 44); \
	A##ki ^= Di; \
	Bbi = rotate_left(A##ki, 43); \
	A##mo ^= Do; \
	Bbo = rotate_left(A##mo, 21); \
	A##su ^= Du; \
	Bbu = rotate_left(A##su, 14); \
	E##ba =   Bba ^ (Bbe | Bbi); \
	E##ba ^= _roundConstants[i]; \
	E##be =   Bbe ^ ((~Bbi) | Bbo); \
	E##bi =   Bbi ^ (Bbo & Bbu); \
	E##bo =   Bbo ^ (Bbu | Bba); \
	E##bu =   Bbu ^ (Bba & Bbe); \
\
	A##bo ^= Do; \
	Bga = rotate_left(A##bo, 28); \
	A##gu ^= Du; \
	Bge = rotate_left(A##gu, 20); \
	A##ka ^= Da; \
	Bgi = rotate_left(A##ka, 3); \
	A##me ^= De; \
	Bgo = rotate_left(A##me, 45); \
	A##si ^= Di; \
	Bgu = rotate_left(A##si, 61); \
	E##ga =   Bga ^ (Bge | Bgi); \
	E##ge =   Bge ^ (Bgi & Bgo); \
	E##gi =   Bgi ^ (Bgo |(~Bgu)); \
	E##go =   Bgo ^ (Bgu | Bga); \
	E##gu =   Bgu ^ (Bga & Bge); \
\
	A##be ^= De; \
	Bka = rotate_left(A##be, 1); \
	A##gi ^= Di; \
	Bke = rotate_left(A##gi, 6); \
	A##ko ^= Do; \
	Bki = rotate_left(A##ko, 25); \
	A##mu ^= Du; \
	Bko = rotate_left(A##mu, 8); \
	A##sa ^= Da; \
	Bku = rotate_left(A##sa, 18); \
	E##ka =   Bka ^ (Bke | Bki); \
	E##ke =   Bke ^ (Bki & Bko); \
	E##ki =   Bki ^ ((~Bko) & Bku); \
	E##ko = (~Bko) ^ (Bku | Bka); \
	E##ku =   Bku ^ (Bka & Bke); \
\
	A##bu ^= Du; \
	Bma = rotate_left(A##bu, 27); \
	A##ga ^= Da; \
	Bme = rotate_left(A##ga, 36); \
	A##ke ^= De; \
	Bmi = rotate_left(A##ke, 10); \
	A##mi ^= Di; \
	Bmo = rotate_left(A##mi, 15); \
	A##so ^= Do; \
	Bmu = rotate_left(A##so, 56); \
	E##ma =   Bma ^ (Bme & Bmi); \
	E##me =   Bme ^ (Bmi | Bmo); \
	E##mi =   Bmi ^ ((~Bmo) | Bmu); \
	E##mo = (~Bmo) ^ (Bmu & Bma); \
	E##mu =   Bmu ^ (Bma | Bme); \
\
	A##bi ^= Di; \
	Bsa = rotate_left(A##bi, 62); \
	A##go ^= Do; \
	Bse = rotate_left(A##go, 55); \
	A##ku ^= Du; \
	Bsi = rotate_left(A##ku, 39); \
	A##ma ^= Da; \
	Bso = rotate_left(A##ma, 41); \
	A##se ^= De; \
	Bsu = rotate_left(A##se, 2); \
	E##sa =   Bsa ^ ((~Bse) & Bsi); \
	E##se = (~Bse)^ (Bsi | Bso); \
	E##si =   Bsi ^ (Bso & Bsu); \
	E##so =   Bso ^ (Bsu | Bsa); \
	E##su =   Bsu ^ (Bsa & Bse);

#define SetColumn(T, S, offset)\
	uint64_t& T##a = S[offset + 0];\
	uint64_t& T##e = S[offset + 1];\
	uint64_t& T##i = S[offset + 2];\
	uint64_t& T##o = S[offset + 3];\
	uint64_t& T##u = S[offset + 4];

#define SetState(T, S)\
	SetColumn(T##b, S,  0);\
	SetColumn(T##g, S,  5);\
	SetColumn(T##k, S, 10);\
	SetColumn(T##m, S, 15);\
	SetColumn(T##s, S, 20);

#define TempColumn(T)\
	uint64_t T##a = 0, T##e = 0, T##i = 0, T##o = 0, T##u = 0;

#define TempState(T)\
	TempColumn(T##b);\
	TempColumn(T##g);\
	TempColumn(T##k);\
	TempColumn(T##m);\
	TempColumn(T##s);

void KeccakF1600::keccup(State& state, bool full) throw()
{
	uint64_t* _state = reinterpret_cast<uint64_t*>(state.data());
	SetState(A, _state);
	TempState(B);
	TempState(E);
	TempColumn(C);
	TempColumn(D);
	
	// Invert lanes 1, 2, 8, 12, 17 and 20
	Abe = ~Abe;
	Abi = ~Abi;
	Ago = ~Ago;
	Aki = ~Aki;
	Ami = ~Ami;
	Asa = ~Asa;
	
	// Prepare Theta
	Ca = Aba ^ Aga ^ Aka ^ Ama ^ Asa;
	Ce = Abe ^ Age ^ Ake ^ Ame ^ Ase;
	Ci = Abi ^ Agi ^ Aki ^ Ami ^ Asi;
	Co = Abo ^ Ago ^ Ako ^ Amo ^ Aso;
	Cu = Abu ^ Agu ^ Aku ^ Amu ^ Asu;
	
	// Rounds 1 … 12
	if(full) {
		thetaRhoPiChiIotaPrepareTheta( 0, A, E)
		thetaRhoPiChiIotaPrepareTheta( 1, E, A)
		thetaRhoPiChiIotaPrepareTheta( 2, A, E)
		thetaRhoPiChiIotaPrepareTheta( 3, E, A)
		thetaRhoPiChiIotaPrepareTheta( 4, A, E)
		thetaRhoPiChiIotaPrepareTheta( 5, E, A)
		thetaRhoPiChiIotaPrepareTheta( 6, A, E)
		thetaRhoPiChiIotaPrepareTheta( 7, E, A)
		thetaRhoPiChiIotaPrepareTheta( 8, A, E)
		thetaRhoPiChiIotaPrepareTheta( 9, E, A)
		thetaRhoPiChiIotaPrepareTheta(10, A, E)
		thetaRhoPiChiIotaPrepareTheta(11, E, A)
	}
	
	// Rounds 13 … 24
	thetaRhoPiChiIotaPrepareTheta(12, A, E)
	thetaRhoPiChiIotaPrepareTheta(13, E, A)
	thetaRhoPiChiIotaPrepareTheta(14, A, E)
	thetaRhoPiChiIotaPrepareTheta(15, E, A)
	thetaRhoPiChiIotaPrepareTheta(16, A, E)
	thetaRhoPiChiIotaPrepareTheta(17, E, A)
	thetaRhoPiChiIotaPrepareTheta(18, A, E)
	thetaRhoPiChiIotaPrepareTheta(19, E, A)
	thetaRhoPiChiIotaPrepareTheta(20, A, E)
	thetaRhoPiChiIotaPrepareTheta(21, E, A)
	thetaRhoPiChiIotaPrepareTheta(22, A, E)
	thetaRhoPiChiIota(23, E, A)
	
	// Invert lanes 1, 2, 8, 12, 17 and 20
	Abe = ~Abe;
	Abi = ~Abi;
	Ago = ~Ago;
	Aki = ~Aki;
	Ami = ~Ami;
	Asa = ~Asa;
}
