CXXFLAGS := -std=c++23 -Wpedantic -Wextra -Wall

SRC   := $(shell pwd)/src
BIN   := $(shell pwd)/bin

TOOL := tool.cc

BINARIES := jar java javac javadoc javap jcmd jconsole jdb jdeps jinfo jmap jmod jps jshell jstack jstat keytool

define build_binary
${CXX} ${CXXFLAGS} -o "${BIN}/${@}" "${SRC}/${TOOL}"
endef

.DEFAULT_GOAL=all
.PHONY: all
all: ${BINARIES}
	@echo ""
	@echo "Please add ${BIN} to your PATH"
	@echo "i.e."
	@echo "	export PATH=\"\$${PATH}:${BIN}\""
	@echo ""

${BIN}:
	@mkdir -p "${BIN}"

${BINARIES}: | ${BIN}
	$(build_binary)

.PHONY: clean
clean:
	@rm -rf "${BIN}"
