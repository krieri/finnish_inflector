#include <iostream>
#include <string>
#include <vector>

#include "header.h"


std::string applyVowelHarmony(std::string& stem, std::string morpheme) {
	for (auto ri = stem.rbegin(); ri != stem.rend(); ++ri) {
		if (isFrontVowel(*ri)) {
			for (char& c : morpheme) {
				if		(c == 'a')	c = 132;
				else if (c == 'o')	c = 148;
				else if (c == 'u')	c = 'y';
			}
			break;
		}
		else if (isBackVowel(*ri))	break;
	}
	return morpheme;
}



void applyConsonantGradation(std::string& stem, bool weaken) {
	// get the gradeable consonant cluster (vowel-final stem and max two consonants per cluster assumed)
	int ppos = stem.length() - 2;
	// assert ppos > 0
	std::string cluster = isVowel(stem[ppos - 1]) ? stem.substr(ppos, 1) : stem.substr(ppos - 1, 2);
	
	int clen = cluster.length();
	auto ASet = weaken ? STRONG_SET : WEAK_SET;	// set to check for matches
	auto BSet = weaken ? WEAK_SET : STRONG_SET; // set to get replacement from
	for (size_t i = 0; i < ASet.size(); ++i) {
		if (cluster == ASet[i]) {
			stem.replace(stem.length() - 1 - clen, clen, BSet[i]);
			break;
		}
	}
	return;
}

std::string merge(std::string stem, std::string& morpheme) { // assuming stem is strong by default
	size_t mlen = morpheme.length();
	if ((mlen == 1 && !isVowel(morpheme[0]))  ||  mlen > 2 && !isVowel(morpheme[0]) && !isVowel(morpheme[1])) {
		applyConsonantGradation(stem, true); // weakens stem when the joined morpheme has closed syllable structure
	}
	return stem.append(applyVowelHarmony(stem, morpheme));
}

std::string merge(std::string stem, const char* morpheme) {
	std::string mstr(morpheme);
	return merge(stem, mstr);
}

std::vector<size_t> syllables(std::string& seg) {
	std::vector<size_t> indices{ 0 };
	size_t len = seg.length();
	for (size_t i = 0; i < len;) {
		for		(; i < len && !isVowel(seg[i]); ++i);
		for		(; i < len &&  isVowel(seg[i]); ++i);
		if		(i + 1 < len &&  isVowel(seg[i + 1]))	indices.push_back(i);
		else if (i + 2 < len && !isVowel(seg[i + 1]))	indices.push_back(i + 1);
	}
	return indices;
}

bool askIf(const char* what) {
	char ans = '\0';
	while (ans != 'y' && ans != 'n') {
		std::cout << "Is the word " << what << "? (y/n)\n";
		std::cin >> ans;
	}
	return ans == 'y' ? true : false;
}

bool isParticiple(std::string& nomSG) {
	// Curiously, "ainut" is the only known word this function misclassifies, being the only
	// word in wiktionary's ohut-type nominals page ending on nUt, making it indistinguishable
	// from the active past participle form of a nonexisting verb ("aida")
	size_t fi = nomSG.length() - 1;
	std::string pre2 = nomSG.substr(fi - 3, 2);
	if ((isU(nomSG[fi - 1] && nomSG[fi] == 't') && (nomSG[fi - 2] == 'n' || pre2 == "ll" || pre2 == "ss"))
			||  (nomSG[fi - 1] == 't' && isU(nomSG[fi]))
			||  (nomSG[fi - 1] == 'v' && isA(nomSG[fi]))) {
		return true;
	}
	return false;
}

DeclensionKey* dKeyGen(std::string& nomSG) {
	
	static DeclensionKey key;
	size_t len = nomSG.length();
	char c1 = nomSG[len - 1];
	char c2 = nomSG[len - 2];

	std::string fin3 = nomSG.substr(len - 3, 3);
	std::string fin2 = nomSG.substr(len - 2, 2);
	std::string	fin1 = nomSG.substr(len - 1, 1);
	std::string vStem3 = nomSG.substr(0, len - 3);
	std::string vStem2 = nomSG.substr(0, len - 2);
	std::string vStem1 = nomSG.substr(0, len - 1);

	std::vector<size_t> syl = syllables(nomSG);

	if (isVowel(c1) && isVowel(c2)) {		// DOUBLE VOWEL TYPE
		dInit(key.pattern, VV_PARADIGM);
		if (c1 == c2) {
			if (c1 == 'e' && askIf("native"))		key.expr2 = "t";
			else if (syl.size() == 1)				dDisable(key.pattern, 1, 0, 2);
			else if (isA(c1))						dEnable(key.pattern, 2, 0, 0);
			else if (!(isU(c2) && c1 == 'e')) {	
				dDisable(key.pattern, 1, 0, 2);
				if (syl.size() == 1 && c1 != 'i')		key.expr1 = ".";
				return &key;
			}
			key.expr1 = "_";
		}
	}
	else if (!isVowel(c1) || c1 == 'i' || c1 == 'e') {	// CANDIDATE FOR MASKED TYPE
		dInit(key.pattern, M_PARADIGM);

		if (fin3 == "nen") {
			key = { vStem3, fin3, "se", "", "_" };
		}
		else if (fin3 == "mpi") {
			key = { vStem1 + "V", fin1, "a" };
			dEnable(key.pattern, 2, 0, 0);
		}
		else if (fin3 == "ton" || fin3 == "t�n") {
			key = { vStem1, fin1, "ma" };
		}
		else if (fin3 == "tar" || fin3 == "t�r") {
			key = { vStem1, fin1, "re" };
		}
		else if (fin2 == "in" || nomSG == "vasen") { // vasen being the only superlative not ending in -in
			if (askIf("a noun"))	key = { vStem1, fin1, "me" };
			else {
				key = { vStem1, fin1, "mpa" };
				dEnable(key.pattern, 2, 0, 0);
			}
		}
		else if (fin1 == "s" && isVowel(fin2[0])) {
			key = { vStem1, fin1 };//, fin2[0]
		}
		else if (isU(fin2[0]) && fin1 == "t") {
			if (isParticiple(nomSG)) {
				key = { vStem2 + "V", fin1, "ee" };
			}
			else {
				key = { vStem1, fin1, "e" };
			}
		}
		else if (fin1 == "e" && askIf("native")) {
			key = { nomSG, "C", "e" };
		}
		else if (fin1 == "i" && syl.size() < 3 && askIf("native")) {
			key.stem = vStem1 + "V";
			key.mask = fin1;
			key.face = "e";
		}
		applyConsonantGradation(key.stem, false);
	}
	else {
		key = { nomSG, "", "" };
		if (fin1 == "i") {
			key.expr1 = "e";
		}
		else if (isO(c1) || isU(c1) || c1 == 'e') {
		}
	}
	return &key;
}

std::string nominative(DeclensionKey& key, bool plural) {
	return plural ? merge(merge(key.stem, key.face), "t") : merge(key.stem, key.mask);
}
std::string partitive(DeclensionKey& key, bool plural) {
	return nullptr;
}
std::string genitive(DeclensionKey& key, bool plural) {
	return nullptr;
}
std::string accusative(DeclensionKey& key, bool plural) {
	return nullptr;
}
std::string inessive(DeclensionKey& key, bool plural) {
	return nullptr;
}
std::string illative(DeclensionKey& key, bool plural) {
	return nullptr;
}
std::string elative(DeclensionKey& key, bool plural) {
	return nullptr;
}
std::string adessive(DeclensionKey& key, bool plural) {
	return nullptr;
}
std::string allative(DeclensionKey& key, bool plural) {
	return nullptr;
}
std::string ablative(DeclensionKey& key, bool plural) {
	return nullptr;
}
std::string essive(DeclensionKey& key, bool plural) {
	return nullptr;
}
std::string translative(DeclensionKey& key, bool plural) {
	return nullptr;
}
std::string instructive(DeclensionKey& key, bool plural) {
	return nullptr;
}
std::string abessive(DeclensionKey& key, bool plural) {
	return nullptr;
}
std::string comitative(DeclensionKey& key, bool plural) {
	return nullptr;
}

void listAllForms(DeclensionKey& key) {

}


ConjugationKey* cKeyGen(std::string& inf1) {
	return nullptr;
}

std::string infinitive(ConjugationKey& key, int infNr) {
	return nullptr;
}
std::string indicative(ConjugationKey& key, int persNr, bool past) {
	return nullptr;
}
std::string conditional(ConjugationKey& key, int persNr) {
	return nullptr;
}
std::string potential(ConjugationKey& key, int persNr) {
	return nullptr;
}
std::string imperative(ConjugationKey& key, int persNr) {
	return nullptr;
}
std::string participle(ConjugationKey& key, bool passive, bool past) {
	return nullptr;
}

void listAllForms(ConjugationKey& key) {

}