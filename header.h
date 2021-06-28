#pragma once

#include <string>
#include <vector>


// VOWEL-RELATED

inline bool isVowel		(char c)	{ return c == 'a' || c == 'e' || c == 'i' || c == 'o' ||
										     c == 'u' || c == 'y' || c == 132 || c == 148 || c == 'V'; }
inline bool	isFrontVowel(char c)	{ return c == 'y' || c == 132 || c == 148; }
inline bool	isBackVowel	(char c)	{ return c == 'u' || c == 'a' || c == 'o'; }
inline bool isA			(char c)	{ return c == 'a' || c == 132; }
inline bool isO			(char c)	{ return c == 'o' || c == 148; }
inline bool isU			(char c)	{ return c == 'u' || c == 'y'; }

// in-place modification of suffix to be in harmony with the remaining part of the composite.
// Returns the suffix with correct vowels.
// all suffixing morphemes are assumed to be in the front group by default (e.g. "ssa", "ko", "nut")
std::string applyVowelHarmony(std::string& stem, std::string morpheme);


// CONSONANT-RELATED

// Clusters qualifying for consonant gradation. Complements share indices.
static const std::vector<std::string> STRONG_SET{ "p", "pp", "mp", "t", "tt", "nt", "lt", "rt", "k", "kk", "nk", "lk", "rk" };
static const std::vector<std::string> WEAK_SET	{ "v", "p",  "mm", "d", "t",  "nn", "ll", "rr", "",  "k",  "ng", "lj", "rj" };
// Strong k is a problematic case. It may disappear, from which there is no simple inverse,
// or turn into v, which is more commonly linked to strong p.
 
// Strengthening is needed when a stem is weak in nominative singular (i.e. its base / dictionary form),
// which is the case for consonant-final and most native e-final nominals (the latter retaining their weak stem
// even though the final consonant has been dropped)

void applyConsonantGradation(std::string& stem, bool weaken);


// MISC

std::string merge(std::string stem, std::string& morpheme);
std::string merge(std::string stem, const char* morpheme);
std::vector<size_t> syllables(std::string& seg); // contains indices to the beginning of each syllable
bool askIf(const char* what);
bool isParticiple(std::string& nomSG);


// DECLENSION OF NOMINALS

// Main declension paradigms, corresponding to the rows of the nominal inflection table.
// from most significant bit:	default ending ((i/j)en / (i/j)A / iin), extended ending (iden / itA / ihin),
//								face (primary inflection stem), expression 1 (alternative inflection stem), expression 2 (etc.),
// 1 = allowed, 0 = prohibited
											// partitive genitive illative	(plurals)
static const unsigned int	SS_PARADIGM[]	= { 0b10100, 0b10010, 0b01010 };
static const unsigned int	 M_PARADIGM[]	= { 0b10011, 0b10010, 0b10010 };
static const unsigned int	SL_PARADIGM[]	= { 0b11110, 0b11010, 0b01010 };
static const unsigned int	VV_PARADIGM[]	= { 0b01010, 0b01010, 0b11010 };

inline void dInit(unsigned int p[], const unsigned int P[]) {
	for (size_t i = 0; i < 3; ++i)			p[i] = P[i];
}
inline void dEnable(unsigned int p[], unsigned int n, size_t start, size_t end) {
	for (size_t i = start; i < end; ++i)	p[i] |= 1UL << n;
}
inline void dDisable(unsigned int p[], unsigned int n, size_t start, size_t end) {
	for (size_t i = start; i < end; ++i)	p[i] &= ~(1UL << n);
}

struct DeclensionKey {
	std::string	 stem;		// shared segment of the nominative singular and primary inflection stem
	std::string	 mask;		// unique segment of the nominative singular
	std::string	 face;		// unique segment of the primary inflection stem
	std::string	 expr1;		// alternative inflection form
	std::string	 expr2;		// alternative inflection form
	unsigned int pattern[3];// tailored variant of one of the paradigms listed above
};
// Examples:
//				lemma		|	stem	|	mask	|	face
//				soitin		|	soitti	|	n		|	me
//				tytär		|	tyttä	|	r		|	re
//				ovi			|	ovV		|	i		|	e		(V = virtual vowel)
//				hylje		|	hylke	|	C		|	e		(C = virtual consonant)

DeclensionKey*	dKeyGen			(std::string& nomSG);

std::string		nominative		(DeclensionKey& key, bool plural);
std::string		partitive		(DeclensionKey& key, bool plural);
std::string		genitive		(DeclensionKey& key, bool plural);
std::string		accusative		(DeclensionKey& key, bool plural);
std::string		inessive		(DeclensionKey& key, bool plural);
std::string		illative		(DeclensionKey& key, bool plural);
std::string		elative			(DeclensionKey& key, bool plural);
std::string		adessive		(DeclensionKey& key, bool plural);
std::string		allative		(DeclensionKey& key, bool plural);
std::string		ablative		(DeclensionKey& key, bool plural);
std::string		essive			(DeclensionKey& key, bool plural);
std::string		translative		(DeclensionKey& key, bool plural);
std::string		instructive		(DeclensionKey& key, bool plural);
std::string		abessive		(DeclensionKey& key, bool plural);
std::string		comitative		(DeclensionKey& key, bool plural);

void			listAllForms	(DeclensionKey& key);




// CONJUGATION OF VERBS

// P-branch:	basis for non-past indicative forms and infinitives 3-5.
// I-branch:	basis for past indicative forms.
// N-branch:	basis for potential forms and active past participles.
// D-branch:	basis for infinitives 1-2 and the present passive.
// T-branch:	basis for all other passive forms including passive past participles.
// C-branch:	basis for active conditional forms. Corresponds to P for the T and NULL type, and I for the D and LS type.

// T-type:		verbs with 1st infinitive stem (D-branch) ending in -t (e.g. verb type 4, 5, 6) except those ending in -st.
// NULL-type:	verbs with 1st infinitive stem (D-branch) ending in a vowel (e.g. verb type 1).
// D-type:		verbs with 1st infinitive stem (D-branch) ending in -d (e.g. verb type 2).
// LS-type:		verbs with 1st infinitive stem (D-branch) ending in -l (e.g. verb type 4) plus those ending in -st.
												 //   P    I     N     D    T
static const std::vector<std::string>    T_PARADIGM{ "a", "si", "nn", "t", "tt" };
static const std::vector<std::string> NULL_PARADIGM{ "",  "i",  "n",  "",  "tt" };
static const std::vector<std::string>    D_PARADIGM{ "",  "i",  "n",  "d", "t" };
static const std::vector<std::string>   LS_PARADIGM{ "e", "i",  ":",  ":", "t" };

struct ConjugationKey {
	char*		pattern[5];		// tailored version of one of the paradigms listed above
	std::string stem;			// starting point for branching
	bool		pConditional;	// C-branch = pConditional ? P-branch : I-branch
};

ConjugationKey*	cKeyGen			(std::string& inf1);
								// persNr 4 := passive, persNr xx := x plural
std::string		infinitive		(ConjugationKey& key, int infNr);
std::string		indicative		(ConjugationKey& key, int persNr, bool past);
std::string		conditional		(ConjugationKey& key, int persNr);
std::string		potential		(ConjugationKey& key, int persNr);
std::string		imperative		(ConjugationKey& key, int persNr);
std::string		participle		(ConjugationKey& key, bool passive, bool past);

void			listAllForms	(ConjugationKey& key);