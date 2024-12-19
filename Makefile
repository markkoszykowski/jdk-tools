CXXFLAGS := -std=c++23 -Wpedantic -Wextra -Wall -g

SRC   := $(shell pwd)/src
BIN   := $(shell pwd)/bin

TOOL := tool.cc

BINARIES := java javac

.DEFAULT_GOAL=all
.PHONY: all
all: $(BINARIES)
	@echo ""
	@echo "Please add ${BIN} to your PATH"
	@echo "i.e."
	@echo "	export PATH=\"\$${PATH}:${BIN}\""

define build_binary
${CXX} ${CXXFLAGS} -o "${BIN}/${@}" "${SRC}/${TOOL}"
endef

$(BIN):
	@mkdir -p "${BIN}"

$(BINARIES): | $(BIN)
	$(build_binary)

.PHONY: clean
clean:
	@rm -rf "${BIN}"
