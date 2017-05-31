cants_archs=( 2 2 2 5 5 10 10 20 20 30 30)
cant_words=( 75 100 200 100 200 100 200 100 200 100 200)
for((i = 0 ; i < ${#cants_archs[@]}; i+=1));do
	python testing01.py ${cants_archs[i]} ${cant_words[i]}
	./test-9 ${cants_archs[i]} ${i} ${cant_words[i]}
done

