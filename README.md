# Linux_Shell

Girilen komutlar eşzamanlı(paralel) olarak çalışmaktadırlar.

	BATCH MOD

	a) Eğer başlangıçta ./Shell ornek.txt  harici mesela  ./Shell ornek.txt ornek2.txt  gibi bir çalıştırma denenirse program çok fazla başlangıç argümanı gibi bir hata mesajı yazdırarak sonlanır.
	b) Verilen txt bulunamaz veya açılamaz ise program hata mesajı verip sonlanır.
	c) Dosyanın içerisinde 512 satırdan fazla (boş satırlar sayılmaz) satırlık komut bulunuyor ise program uygun bir hata mesajı yazdırarak sonlanır.	
	d) Dosyada "quit" komutu görülürse veya okuma dosya sonuna gelirse program kendiliğinden sonlanır.

	İNTERAKTİF MOD

	a) Boş komut satırında ENTER'a basılması bir hata mesajı yazdırmaz ve programı etkilemez.
	b) "quit" komutu girilirse program bir hata mesajı vermeden sonlanır.
	c) 'Ctrl-D' tuşlarına basılırsa çalışan process yarıda kesilip program sonlanır. 

	GENEL

	a) Bir satıra 512 karakterden uzun bir input girilmeye çalışılırsa program uygun bir hata mesajı yazdırarak çalışmaya devam eder.
	b) Bir seferde işlenebilecek komut sayısı yani bir satıra yazılabilecek max komut sayısı 16'dır (argümanlar hariç). Bu sınırın aşılması durumunda program uygun bir hata mesajı yazar ve çalışmaya devam eder.
	c) Bir komutun max 63 argümanı bulunabilir. Bu sınır aşılırsa program uygun bir hata mesajı yazar ve çalışmaya devam eder.
	d) Bir satır ';' ile başlar ise veya arasında komut olmadan ard arda iki ';' kullanılmış ise bu programımız için bir syntax hatasıdır ve program buna uygun bir mesaj yazdırarak çalışmaya devam eder.
	e) Çalıştırılmak istenen komut çalıştırılamaz veya yok ise uygun bir hata mesajı yazdırılarak program çalışmaya devam eder.
	f) Komut satırında fazladan boşluklar bulunması program için bir şey ifade etmez ve program komutlarda ve argümanlarda sorun olmadıkça çalışmaya devam eder.
	g) Programın çalışması sırasında herhangi bir bellek tahsis (malloc) hatasıyla karşılaşılırsa program bir hata mesajı yazdırarak sonlanır.
	h) Çalıştırılacak komut için child process oluşturulamaz ise program bir hata mesajıyla bunu belirtir,komut gerçekleştirilemez fakat program çalışmaya devam eder.
	i) Programda "cd" komutu kullanılabilir hale getirilmiştir böylece directory değiştirilebilir. Ayrıca prompt yazısında kullanıcının hangi directory'de olduğu görünebilir.
