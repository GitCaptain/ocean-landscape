import os
from collections import defaultdict
import matplotlib.pyplot as plt
import numpy as np

def process_whole(directory, files):
    measurements = defaultdict(lambda: defaultdict(list))
    years = []
    sizes = []
    for f in files:
        sz, year = map(int, f.split('_')[1:3])
        sizes.append(sz)
        years.append(year)
        with open(os.path.join(directory, f)) as data:
            measurements[sz][year] = sorted(map(float, data.readline().strip().split()))

    sizes = sorted(set(sizes))
    years = sorted(set(years))

    fix_size = sizes[-1]
    plt.figure(figsize=(10, 6))
    plt.title(f'Зависимость времени выполнения от количества лет (размер сетки: {fix_size})')
    plt.xlabel('Количество лет')
    plt.ylabel('Время выполнения (секунды)')
    values = [np.mean(measurements[fix_size][y]) for y in years]
    values[1], values[2] = values[2], values[1]
    plt.plot(years, values, marker='o')
    file_name = f'graphs/whole_mean_per_size_{sz}.png'
    plt.savefig(file_name)
    plt.close()

    fix_year = years[-1]
    plt.figure(figsize=(10, 6))
    plt.title(f'Зависимость времени выполнения от размера сетки(количество лет: {fix_year})')
    plt.xlabel('размер сетки')
    plt.ylabel('Время выполнения (секунды)')
    data = [np.mean(measurements[_][fix_year]) for _ in sizes]
    plt.plot(sizes, data, marker='o')
    file_name = f'graphs/whole_mean_per_year_{sz}.png'
    plt.savefig(file_name)
    plt.close()


def process_units(directory, files):
    measurements = defaultdict(lambda: defaultdict(lambda: defaultdict(list)))
    years = []
    sizes = []
    units = []
    for f in files:
        try:
            unit, sz, year = f.rsplit('_', 2)
        except:
            continue
        sz, year = int(sz), int(year)
        if unit == 'set_height':
            unit = 'noise'
        units.append(unit)
        sizes.append(sz)
        years.append(year)
        with open(os.path.join(directory, f)) as data:
            measurements[unit][sz][year] = \
                sorted(map(float, data.readline().strip().split()))

    sizes = sorted(set(sizes))
    years = sorted(set(years))
    units = list(set(units))

    fix_size = sizes[-1]
    plt.figure(figsize=(10, 6))
    plt.title(f'Зависимость времени выполнения от количества лет (размер сетки: {fix_size})')
    plt.xlabel('Количество лет')
    plt.ylabel('Время выполнения (секунды)')
    for unit in measurements:
        values = []
        for y in years:
            v = np.mean(measurements[unit][fix_size][y])
            values.append(v)
        plt.plot(years, values, label=unit)
    plt.legend()
    file_name = f'graphs/units_mean_per_year_sz_{fix_size}.png'
    plt.savefig(file_name)
    plt.close()

    fix_year = years[-1]
    plt.figure(figsize=(10, 6))
    plt.title(f'Зависимость времени выполнения от размера сетки(количество лет: {fix_year})')
    plt.xlabel('размер сетки')
    plt.ylabel('Время выполнения (секунды)')
    for unit in measurements:
        data = []
        for sz in sizes:
            mean = np.mean(measurements[unit][sz][fix_year])
            data.append(mean)
        plt.plot(sizes, data, label=unit)
    plt.legend()
    file_name = f'graphs/units_mean_per_size_y_{fix_year}.png'
    plt.savefig(file_name)
    plt.close()

    # fix_year = years[-1]
    # relief_units = ('ContinentalMargin', 'DeepSeaBasin', 'MidOceanRidge')
    # init = 'Init'
    # iteration = 'Iteration'
    # plt.figure(figsize=(10, 6))
    # plt.title(f'Зависимость времени инициализации и итерации элемента от размера сетки')
    # x = np.asarray(sizes)
    # bar_width = 6
    # for i, unit in enumerate(relief_units):
    #     init_times = [np.mean(measurements[f'{unit}{init}'][_][fix_year]) for _ in sizes]
    #     iteration_times = [np.mean(measurements[f'{unit}{iteration}'][_][fix_year]) for _ in sizes]
    #     bar_x = x + (bar_width/3)*(i-1)
    #     plt.bar(
    #         bar_x, iteration_times,
    #         width=bar_width/3,
    #         label=f'{unit}_it',
    #         color='b',
    #         align='center')
    #     plt.bar(
    #         bar_x, init_times,
    #         width=bar_width/3,
    #         label=f'{unit}_init',
    #         color='r',
    #         align='center',
    #         bottom=iteration_times)
    # file_name = f'graphs/relief_elements_per_size.png'
    # plt.legend()
    # plt.savefig(file_name)
    # plt.close()

    fix_year = years[-1]
    unit = 'MidOceanRidge'
    init = 'Init'
    iteration = 'Iteration'
    _, ax1 = plt.subplots()
    ax1.set_title(f'Зависимость времени инициализации и итерации СОХ от размера сетки')
    init_times = [np.mean(measurements[f'{unit}{init}'][_][fix_year]) for _ in sizes]
    iteration_times = [np.mean(measurements[f'{unit}{iteration}'][_][fix_year]) for _ in sizes]
    ax1.set_ylabel('iterations')
    ax1.set_xlabel('size')
    ax1.plot(
        sizes, iteration_times,
        label=f'{unit}_iterations',
        color='blue')
    ax2 = ax1.twinx()
    ax2.set_ylabel('init')
    ax2.plot(
        sizes, init_times,
        label=f'{unit}_init',
        color='red')
    file_name = f'graphs/relief_elements_per_size.png'
    # plt.legend()
    ax1.legend(loc='upper left')
    ax2.legend(loc='upper right')
    plt.savefig(file_name)
    plt.close()

    fix_year = years[-1]
    unit = 'DeepSeaBasin'
    init = 'Init'
    iteration = 'Iteration'
    plt.figure(figsize=(10, 6))
    plt.title(f'Зависимость времени инициализации и итерации ГК от размера сетки')
    init_times = [np.mean(measurements[f'{unit}{init}'][_][fix_year]) for _ in sizes]
    iteration_times = [np.mean(measurements[f'{unit}{iteration}'][_][fix_year]) for _ in sizes]
    plt.plot(
        sizes, iteration_times,
        label=f'{unit}_iterations',
        color='blue')
    plt.plot(
        sizes, init_times,
        label=f'{unit}_init',
        color='red')
    file_name = f'graphs/dsb_elements_per_size.png'
    plt.legend()
    plt.savefig(file_name)
    plt.close()


def main():
    results_dir = 'results'
    whole_measure_files = []
    unit_measure_files = []
    for f in os.listdir(results_dir):
        if f.endswith('_whole'):
            whole_measure_files.append(f)
        else:
            unit_measure_files.append(f)
    process_whole(results_dir, whole_measure_files)
    process_units(results_dir, unit_measure_files)


if __name__ == '__main__':
    main()
