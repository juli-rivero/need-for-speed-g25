.PHONY: all tests clean editor client server build exec hooks

# compile-debug:
#	mkdir -p build/
#	cmake -S . -B ./build -DCMAKE_BUILD_TYPE=Debug $(EXTRA_GENERATE)
#	cmake --build  build/ $(EXTRA_COMPILE)

build:
	cmake --build  build/

tests: build
	./build/taller_tests

all: clean tests

clean:
	rm -Rf build/

editor: build
	./build/taller_editor
client: build
	./build/taller_client
server: build
	./build/taller_server


.PHONY: hooks

hooks:
ifeq ($(MODE),fast)
	pre-commit run trailing-whitespace --all-files ; \
	pre-commit run end-of-file-fixer --all-files ; \
	pre-commit run check-yaml --all-files ; \
	pre-commit run clang-format --all-files ; \
	pre-commit run cppcheck --all-files ; \
	pre-commit run cpplint --all-files ;
else
	pre-commit run --all-files
endif

exec: build
	gnome-terminal --title="Servidor" -- bash -c "./build/taller_server; exec bash"
	gnome-terminal --title="Cliente" -- bash -c "./build/taller_client; exec bash"
