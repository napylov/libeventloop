О проекте
==============

libeventloop это С++ - обёртка над libevent.


Лицензия
==============

См. файл LICENSE.ru.txt


Версия
==============

0.0.1-alpha. Нестабильная.


Описание
==============

Подробная документация будет написана позже.
До выпуска стабильной версии обратная совместимость может быть сломана.


Сборка
==============

Перейти в подкаталог build

cd build

Запустить cmake для сборки Makefile

cmake -G "Unix Makefiles" [-DCMAKE_INSTALL_PREFIX:PATH=<install prefix; /usr/local default>] [-DCMAKE_BUILD_TYPE=Debug|Release] ..

Затем запустить make и, опционально, make test.
По окончании - make install для установки.

make
[make test]
make install
