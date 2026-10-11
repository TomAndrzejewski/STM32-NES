#ifndef SOURCES_INC_NES_ASSERT_H_
#define SOURCES_INC_NES_ASSERT_H_

// Checks an assumption that only a bug in the code can break (not the player, not the hardware).
// Active in Release builds as well.
#define NES_ASSERT(cond) \
	do { \
		if (!(cond)) { NES_AssertFailed(__func__, __LINE__); } \
	} while (0)

void NES_AssertFailed(const char* file, int line) __attribute__((noreturn));

#endif /* SOURCES_INC_NES_ASSERT_H_ */