import random

class event_list:

    def __init__(self, filename="working_pmu.txt"):
        with open(filename, 'r') as file:
           self.event_list = [str.strip(s) for s in list(file)]

    def sample(self, num):
        return random.sample(self.event_list, num)

    def sample_range(self, min_e, max_e):
        num = random.randint(min_e,max_e)
        return self.sample(num)

    def random_set(self, num_lists, min_e, max_e):
        return [self.sample_range(min_e, max_e) for _ in range(num_lists)]


if __name__ == "__main__":
    el = event_list()
    ll = el.random_set(20, 10, 50)

    for i,l in enumerate(ll):
        print(f"LIST {i}: count: {len(l)}")
        print(*l, sep='\n')
        print('\n\n')