#ifndef SOURCES_INC_BOARD_ASSERT_H_
#define SOURCES_INC_BOARD_ASSERT_H_

// Checks an assumption that only a bug in the code can break.
// Active in Release builds as well.
#define BOARD_ASSERT(cond) \
	do { \
		if (!(cond)) { BOARD_AssertFailed(__func__, __LINE__); } \
	} while (0)

void BOARD_AssertFailed(const char* func, int line) __attribute__((noreturn));

#endif /* SOURCES_INC_BOARD_ASSERT_H_ */