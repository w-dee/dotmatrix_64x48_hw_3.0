all: large_digits.inc bold_digits.inc week_names.inc

large_digits.inc: large_digits.png make_digits.rb
	ruby make_digits.rb large_digits.png \
		10 14 18 14 0 0 0 15 \
		"'0','1','2','3','4','5','6','7','8','9'" \
		> large_digits.inc

bold_digits.inc: bold_digits.png make_digits.rb
	ruby make_digits.rb bold_digits.png \
		13 6 8 7 0 0 0 6 \
		"'%','.','/','0','1','2','3','4','5','6','7','8','9'" \
		> bold_digits.inc

week_names.inc: week_names.png make_digits.rb
	ruby make_digits.rb week_names.png \
		7 22 8 0 17 0 2 0 \
		"'0','1','2','3','4','5','6'" \
		> week_names.inc


