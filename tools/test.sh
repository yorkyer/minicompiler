for file in `ls ../test/in`; do
	../bin/compiler ../test/in/${file} no > /dev/null
	java -jar ../library/Mars.jar nc mips.txt > ../test/out/$file

	../bin/compiler ../test/in/${file} > /dev/null
	java -jar ../library/Mars.jar nc mips.txt > output.txt

	if diff ../test/out/$file output.txt; then
		echo $file succeed!
	else
		echo $file fail!
	fi
done
