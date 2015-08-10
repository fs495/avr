
test: mml_parser.tab.rb
	ruby mml_parser.tab.rb < test.mml

mml_parser.tab.rb: mml_parser.y
	racc --debug mml_parser.y

clean:
	rm -f mml_parser.tab.rb
