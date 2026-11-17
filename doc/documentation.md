# Projekt – Měření srdečního tepu

### Vysoké učení technické v Brně
### Fakulta informačních technologií
### Mikroprocesorové a vestavěné systémy

* **Autor:** David Švancer (xsvancd00)
* **Email:** xsvancd00@stud.fit.vutbr.cz
* **Datum:** 19. prosince 2025

---
<div style="page-break-after: always;"></div>

## **1. Úvod**

Zadáním projektu byl návrh a implementace vestavné aplikace na platformě **Espressif ESP32**. Cílem bylo vytvořit zařízení pro měření srdečního tepu. 

Systém se skládá z vývojové desky ESP32, analogového senzoru srdečního tepu, grafického OLED displeje a ovládacího tlačítka. Výsledná aplikace měří frekvenci srdečního tepu, filtruje naměřená data a zobrazuje číselnou hodnotu a graf na displeji.

Aplikace byla implementována v jazyce C s využitím aplikačního rámce **ESP-IDF**.

## **2. Zapojení hardware**

Systém využívá periferie mikrokontroléru ESP32 pro komunikaci s externími moduly.

### **2.1 Modul pro měření srdečního tepu**

Pro snímání tepu byl použit analogový senzor. Senzor obsahuje LED diodu a fototranzistor.
Princip spočívá v tom, že fototranzistor snímá odrážené světlo, které převádí na analogový signál. Změna prokrvení, ke kterému dochází při běžné činnosti srdce, ovlivňuje množství odráženého světla.

Modul má tři vývody:

* **VCC:** Napájení.
* **GND:** Uzemnění.
* **SIGNAL:** Analogový výstup připojený na ADC převodník mikrokontroléru.

U periferie bylo uzemnění připojeno k uzemnění přímo na mikrokontroléru, napájení na zdroj s 5V a analogový výstup byl připojen k GPIO pinu 34.

### **2.2 Grafický OLED displej**

Pro zobrazení hodnoty tepu a jejího grafu byl využit OLED displej s rozlišením 128x64 pixelů. Komunikace s mikrokontrolérem probíhá přes sběrnici I2C.

Modul má čtyři vývody: 

* **VCC:** Napájení.
* **GND:** Uzemnění.
* **SCL:** Hodinový signál pro komunikaci přes sběrnici I2C.
* **SDA:** Datový signál pro komunikaci přes sběrnici I2C.

U periferie bylo  uzemnění připojeno k uzemnění přímo na mikrokontroléru, napájení na zdroj s 3.3V, hodinový signál k GPIO pinu 22 a datový signál k GPIO pinu 21. Pin 21 je standardní pro datový signál a GPIO pin 22 je standardní pin pro hodinový signál podle dokumentace.

<div style="page-break-after: always;"></div>

### **2.3 Ovládací tlačítko**

K ovládání aplikace slouží tlačítko. Pomocí stlačení tlačítka se spustí měření, po jeho opětovném zmáčknutí se měření přeruší a resetují se všechny hodnoty, které jsou u měření použity tak, aby po dalším stisknutí bylo možné znovu změřit tep nezávisle na předchozích měřeních.

Modul má několik vývodů, byly použity 2:
* **COM:** Společný kontakt tlačítka, připojený k zemi.
* **MID:** Výstupní kontakt tlačítka, připojený k GPIO pinu mikrokontroléru.

U periferie byl výstupní kontakt připojen k GPIO pinu 18 a vzhledem k tomu, že při konfiguraci byl zvolen pull-up rezistor, tak byl společný kontakt připojen k uzemnění na mikrokontroléru. Tlačítko je tedy stlačováno oproti zemi.

### **2.4 Schéma zapojení**

Význam jednotlivých pinů a jejich mapování v rámci ESP32 popisuje následující tabulka:

| Modul | Pin modulu | Pin ESP32 | Funkce v ESP-IDF |
| --- | --- | --- | --- |
| **Senzor** | Signal (S) | GPIO 34 | `ADC1_CHANNEL_6` |
|  | VCC | 5V | Napájení |
|  | GND | GND | Uzemnění |
| **OLED** | SDA | GPIO 21 | `I2C_MASTER_SDA` |
|  | SCL | GPIO 22 | `I2C_MASTER_SCL` |
|  | VCC | 3.3V | Napájení |
| **Tlačítko** | MID | GPIO 18 | `GPIO_NUM_18` |
|  | COM | GND | Spínání k zemnění |

---
<div style="page-break-after: always;"></div>

## **3. Způsob řešení**
Projekt je rozdělen do několika modulů (`.c` a `.h` souborů) pro zvýšení přehlednosti a udržitelnosti kódu. Využívá se operační systém FreeRTOS, který je součástí ESP-IDF, pro časování hlavní smyčky.

### **3.1 Struktura projektu**

* `main.c`: Hlavní smyčka aplikace, řízení stavů mezi měřením a čekáním na začátek měření vyvolaný zmáčknutím tlačítka.
* `senzor.c/h`: Inicializace senzoru, čtení hodnot z analogového senzoru, filtrace signálu a detekce tepů.
* `oled_display.c/h`: Inicializace displeje, zobrazování textu na displeji a zobrazování grafu na displeji.
* `button.c/h`: Inicializace tlačítka a detekce stisku.
* `util.c/h`: Pomocné funkce pro řazení a počítání mediánu.

### **3.2 Zpracování signálu a detekce tepu**

Surová data přicházející z analogového senzoru jsou rušena šumem (nežádoucími hodnotami). Proto je potřeba data filtrovat, abychom mohli získat správné hodnoty tepu. Při implementaci bylo vyzkoušeno více metod pro zpracování příchozích dat (korelační metoda, filtr s dolní propustí, metoda pevného prahu). Metoda, která měřila nejpřesnější hodnoty a nejvíce se osvědčila, byla metoda využívající filtr s dolní propustí v kombinaci s dynamickým prahem. 

V projektu byla metoda implementována následovně:

1. **Filtr dolní propusti**
- Filtr pro vyhlazení vysokofrekvenčního šumu funguje na principu výpočtu nové hodnoty kombinací předcházející a nově naměřené hodnoty, větší důraz dává na předešlou hodnotu. V průběhu implementace bylo vyzkoušeno více možností, ale nejvíce se osvědčila kombinace hodnot: `vyfiltrovaná hodnota = (0.8 * předchozí_hodnota) + (0.2 * aktuální_hodnota)` [[6]](#ref6)

2. **Dynamický práh**
- Pro detekci tepu není použitá pevná hodnota prahu ale dynamická, která se přizpůsobuje přicházejícímu signálu. K této hodnotě je připočtena malá hodnota (hystereze) [[7]](#ref7) pro větší ochranu proti falešným signálům. Algoritmus funguje tak, že detekuje překročení této hodnoty. [[1]](#ref1)

3. **Výpočet tepové frekvence**
- Při detekci tepu se zaznamená aktuální čas. Po druhé detekci tepu jsme schopni vypočítat hodnotu tepové frekvence pomocí vzorce `60 000 / (aktuální_čas_detekce - čas_předchozí_detekce)` [[3]](#ref3).

<div style="page-break-after: always;"></div>

4. **Filtrace BPM**
- Aby bylo zabráněno chybně změřeným hodnotám, ukládají se vypočítané hodnoty tepu do kruhového pole, které je při každé nově přidané hodnotě seřazeno podle jednotlivých hodnot, aby z něho bylo možné vypočítat medián, který je následně zobrazen na displeji. Díky tomu je hodnota srdečního tepu na displeji zobrazena stabilně a chyba při měření silně neovlivní zobrazenou hodnotu.

### **3.3 Vizualizace**

1. **Hodnota tepové frekvence**
- V klidovém stavu se na displeji zobrazí úvodní text. Jakmile je měření zahájeno, je na displeji zobrazena hodnota tepové frekvence, která je při každém novém průchodu aktualizována. Pro zobrazení nové hodnoty na displeji, je nutné nejprve vymazat všechny pixely na displeji nastavením jejich hodnoty na 0, poté zapsat do paměti nové a následně zavolat funkci, která je vykreslí.

2. **Graf**
- Pro zobrazení grafu je využíván algoritmus automatického škálování [[5]](#ref5).
Sleduje se minimální a maximální hodnota signálu v měřeném čase a tyto hodnoty se posouvají postupně k sobě. Díky tomu je graf vždy roztažen na celou výšku vyhrazené oblasti displeje. K zobrazení je využita metoda posuvného registru, při každém novém čtení jsou všechny hodnoty o jednu posunuty, nejstarší hodnota je zahozena a je uložena hodnota nová.

## **4. Video**

1. **Kontext videa**
- Video demonstrující celkovou funkčnost je točeno za pomocí druhé osoby, u které senzor přesněji měří (tenčí prsty). Osoba byla v klidovém režimu těsně před spaním, takže naměřená hodnota kolem 58-60 BPM odpovídá a demonstruje správně funkčnost.

2. **Odkaz na video**
- Pro přístup k videu je potřeba se přihlásit pod účtem organizace VUT. Odkaz: [[8]](#ref8).

## **5. Závěr**

Cílem projektu bylo vytvořit funkční měřič tepu, což se podařilo splnit. Kombinace filtru dolní propusti a algoritmu dynamického prahu zajišťuje spolehlivou detekci tepu i při kolísávém signálu (např. při změně přítlaku prstu nebo pohybu prstu). Pro eliminaci nahodilých chyb měření byl navíc implementován mediánový filtr, který výrazně zvýšil stabilitu zobrazovaných hodnot. Vizualizace pomocí automaticky škálovaného grafu umožňuje uživateli snadno ověřit kvalitu snímaného signálu.

<div style="page-break-after: always;"></div>

## **6. Literatura**

<a id="ref1"></a>[1] World Famous Electronics GitHub (Algoritmus dynamického prahu). Dostupné z: [https://github.com/WorldFamousElectronics/PulseSensor_Amped_Arduino](https://github.com/WorldFamousElectronics/PulseSensor_Amped_Arduino).

<a id="ref2"></a>[2] SOLOMON SYSTECH. SSD1306 Datasheet (nastavení inicializace displeje). Dostupné z: [https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf](https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf).  

<a id="ref3"></a>[3] BPM formula. Dostupné z: [https://pmc.ncbi.nlm.nih.gov/articles/PMC3305903/#:~:text=As%20a%20result%20the%20sequence%20of%20consecutive,to%20formula:%20FHRi%20%5Bbpm%5D%20=%2060000/Ti%20%5Bms%5D](https://pmc.ncbi.nlm.nih.gov/articles/PMC3305903/#:~:text=As%20a%20result%20the%20sequence%20of%20consecutive,to%20formula:%20FHRi%20%5Bbpm%5D%20=%2060000/Ti%20%5Bms%5D.).

<a id="ref4"></a>[4] Espressif Systems. Dostupné z: [https://docs.espressif.com/projects/esp-idf/en/latest/esp32/](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/).

<a id="ref5"></a>[5] Algoritmus automatického škálování. Dostupné z: [https://toptechboy.com/plot-and-data-or-graph-on-any-oled/](https://toptechboy.com/plot-and-data-or-graph-on-any-oled/).

<a id="ref6"></a>[6] Exponenciální vyhlazovaní. Dostupné z: [https://en.wikipedia.org/wiki/Exponential_smoothing](https://en.wikipedia.org/wiki/Exponential_smoothing)

<a id="ref7"></a>[7] Hystereze. Dostupné z: [https://arduino.stackexchange.com/questions/9115/how-to-add-hysteresis-to-threshold-values](https://arduino.stackexchange.com/questions/9115/how-to-add-hysteresis-to-threshold-values)

<a id="ref8"></a>[8] Video demonstrující funkčnost. Dostupné z: [https://vutbr-my.sharepoint.com/:f:/g/personal/xsvancd00_vutbr_cz/IgAtzIcOBgMyQKmMqZCJWdz4AUUrNG6YGDakVTwQ8el3SJk?e=6qn11k](https://vutbr-my.sharepoint.com/:f:/g/personal/xsvancd00_vutbr_cz/IgAtzIcOBgMyQKmMqZCJWdz4AUUrNG6YGDakVTwQ8el3SJk?e=6qn11k)

<a id="ref9"></a>[9] ESPRESSIF SYSTEMS. ESP-IDF Programming Guide (nastavení inicializace GPIO periferií). Dostupné z: [https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/index.html](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/index.html).

<a id="ref10"></a>[10] Schéma zapojení desky. Dostupné z: [https://www.fit.vut.cz/person/simekv/public/IMP_projekt_board_ESP32_Wemos_D1_R32.pdf](https://www.fit.vut.cz/person/simekv/public/IMP_projekt_board_ESP32_Wemos_D1_R32.pdf).