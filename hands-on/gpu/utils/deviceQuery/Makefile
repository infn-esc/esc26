all:
	/usr/local/cuda-13.3/bin/nvcc deviceQuery.cpp -o deviceQuery

run: all
	./deviceQuery

clean:
	rm -f deviceQuery
