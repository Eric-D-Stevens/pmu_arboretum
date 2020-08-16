import os
import event_utils

num_trees = 50
min_events = 10
max_events = 20


util = event_utils.event_list()
event_lists = util.random_set(num_trees,min_events,max_events)

program_dir = './bench1'
programs = os.listdir(program_dir)

for tree_id, e_list in enumerate(event_lists):
    event_string = ' '.join(e_list)
    os.mkdir(f'./data1/t{str(tree_id)}/')
    with open(f'./data1/t{str(tree_id)}/event_list.csv', 'w') as event_file:
        for idx, event in enumerate(e_list):
            event_file.write(f'{idx},\t{event}\n')

    for program in programs:
        run_program = program_dir+f'/{program}' 

        # RUN DATA COLLECTION FOR TREE AND PROGRAM
        os.system(f'sudo ./get_pmu_data {run_program} {event_string}')
        os.system(f'cp ./current_data.csv ./data1/t{str(tree_id)}/{program}.csv')


print(program)