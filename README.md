# Ocean landscape generator: Процедурная генерация подводного рельефа

Это итеративный алгоритм создания подводных ландшафтов, основанный на модели подводных природных процессов, разработанной в ходе моей магистерской диссертации. Он способен генерировать основные типы океанического рельефа, такие как: глубоководные котловины с гайотами, учитывающие процесс абразии, континентальные окраины, учитывающие процесс субдукции, и срединно-океанические хребты, обусловленные рифтогенезом.

С текстом магистерской диссертации можно ознакомиться [здесь](https://github.com/GitCaptain/ocean-landscape/blob/master/data/thesis.pdf)

## Примеры

Ниже приведены два примера сгенерированного ландшафта:

![Пример 1](https://github.com/GitCaptain/ocean-landscape/blob/master/data/examples/landscape.png)

![Пример 2](https://github.com/GitCaptain/ocean-landscape/blob/master/data/examples/noise_relief.png)

## Сборка и запуск

Как собрать генератор:
```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release  ../src/
make LandscapeGenerator
```

Как использовать:
```
./build/LandscapeGenerator --sizex=X --sizey=Y --years=N [ --output=file ] [ --mor-cnt=cnt ] [ --basin-cnt=cnt ] [ --margin-cnt=cnt ]
```

Пример запуска:
```
./LandscapeGenerator --sizex=500 --sizey=500 --years=300000 --output=out.vox
```

## Как посмотреть получившийся ландшафт

Результатом работы алгоритма является файл в формате ".vox", описанный [здесь](https://github.com/ephtracy/voxel-model/tree/master)

Чтобы просмотреть этот файл, вам необходимо скачать бесплатный воксельный редактор "Magical Voxel" с [официального сайта](https://ephtracy.github.io) и
открыть в нем сгенерированный файл.

