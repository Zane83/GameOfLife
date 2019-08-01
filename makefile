#Allegro libraries that have to link
ALLEGRO_OPTIONS := -lallegro -lallegro_primitives

#MPI compiler
CPP := mpic++

#All source files
SRCS := $(shell find src -type f -name "*.cpp")

#All object files
OBJS := $(patsubst src/%,./obj/%.o,$(basename $(SRCS)))

#Creates a directory where put object files
$(shell mkdir -p obj >/dev/null)

gameoflife : $(OBJS)
	$(CPP) $(OBJS) -o gameoflife $(ALLEGRO_OPTIONS)

./obj/%.o : ./src/%.cpp 
	$(CPP) -c $^ -o $@
	
.PHONY: clean
clean:
	rm -r obj
