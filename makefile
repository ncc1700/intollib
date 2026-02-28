gen-comp:
	xmake project -k compile_commands --lsp=clangd

build-win64:
	xmake build intollib-win64


test-win64:
	xmake build intollib-win64
	cp shader.wgsl output/win-amd64/shader.wgsl
	xmake run test-win64

clean:
	rm -rf .cache
	rm -rf .xmake
	rm -rf build
	rm -rf output