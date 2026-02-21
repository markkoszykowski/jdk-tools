CXXFLAGS := -std=c++26 -Wpedantic -Wextra -Wall

SRC   := $(shell pwd)/src
BIN   := $(shell pwd)/bin

TOOL := tool.cc

BINARIES := jar jarsigner java javac javadoc javap jcmd jconsole jdb jdeprscan jdeps jfr jhsdb jimage jinfo jlink jmap jmod jnativescan jpackage jps jrunscript jshell jstack jstat jstatd jwebserver keytool rmiregistry serialver

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
