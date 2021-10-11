.PHONY: format clean

format:
	git ls-files | grep -E '\.(c|cpp|cc|h)$$' | xargs clang-format -i

clean:
	-find . \( -name '*.out' -o -name '*.pdb' -o -name '*.exp' -o -name '*.lib' \) | xargs rm -f
	-find . -name '.vscode' -type d | xargs rm -rf

