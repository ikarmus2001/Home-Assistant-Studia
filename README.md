# Home Assistant - Doniczkowa stacja pomiarowa

W świecie danych i informacji chcemy trzymać rękę na pulsie nawet w najdrobniejszych sprawach.
Moja stacja pomiarowa odczytuje dane z kilku zamontowanych czujników, a następnie za pomocą WiFi komunikuje się
z urządzeniem-mózgiem, wyposażonym w oprogramowanie [Home Assistant](https://www.home-assistant.io/).

W projekcie wykorzystałem Raspberry Pi 4 (na którym zainstalowałem i skonfigurowałem HA) oraz ERP8266, 
które zbierało dane o poziomie wody w doniczce, temperaturze i wilgotności powietrza oraz parsowało i udostepnialo dane w formacie JSON jako WebServer.
Udostępniony został również endpoint, za pomocą którego można było sterować lampką (uruchamianie poprzez switcha obsługiwanego z GPIO).

W utowrzeniu prototypu najbardziej skomplikowane było zainstalowanie oraz konfiguracja obsługi endpointów na Home Assistancie.
Kod sterujący mikrokontrolerem ESP jest bardzo prosty i minimalistyczny.
