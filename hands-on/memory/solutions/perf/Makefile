# -g: line numbers for perf report / perf annotate
# -fno-omit-frame-pointer: call graphs with perf record -g
CXXFLAGS = -std=c++17 -O2 -g -fno-omit-frame-pointer -Wall

PROGRAMS = events field particles channels

all: $(PROGRAMS)

%: %.cc timer.h
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f $(PROGRAMS) perf.data perf.data.old
