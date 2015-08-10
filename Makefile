TARGETS = pin_test1 liblcd printf flan_led1 autopush-hid

all:
	for d in $(TARGETS); do (cd $$d && $(MAKE) all); done

clean:
	for d in $(TARGETS); do (cd $$d && $(MAKE) clean); done
