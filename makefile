a.out:
			g++ -std=c++14 Voronoi/Voronoi/*.cpp

voronoi:a.out
				./a.out
				rm a.out

clean:
			rm output.svg
