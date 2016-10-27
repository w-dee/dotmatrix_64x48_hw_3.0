all: large_digits.h

large_digits.h: large_digits.png make_digits.rb
	ruby make_digits.rb large_digits.png \
		10 14 18 14 0 0 \
		> large_digits.h

