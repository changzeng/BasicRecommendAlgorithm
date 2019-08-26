#encoding: utf-8

from random import randint

user_num = 1000
item_num = 1000
act_num = 1000

if __name__ == "__main__":
	act_set, act_ind = set(), 0
	act_fd = open("user_act.txt", "w")
	while act_ind < act_num:
		user_id = randint(0, user_num-1)
		item_id = randint(0, item_num-1)
		act_str = "{0}_{1}".format(user_id, item_id)
		if act_str in act_set:
			continue
		act_set.add(act_str)
		act_fd.write("{0}\t{1}\n".format(user_id, item_id))
		act_ind += 1
	act_fd.close()
