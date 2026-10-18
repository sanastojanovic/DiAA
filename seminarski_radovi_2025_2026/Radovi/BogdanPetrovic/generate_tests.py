import random
import string

def generate_test_data(dict_size=10000, test_size=1000):
    words = set()
    while len(words) < dict_size:
        length = random.randint(3, 10)
        word = ''.join(random.choices(string.ascii_lowercase, k=length))
        words.add(word)

    with open('dictionary.txt', 'w') as f:
        for word in words:
            freq = random.randint(1, 100000)
            f.write(f"{word} {freq}\n")

    word_list = list(words)
    with open('test.txt', 'w') as f:
        for _ in range(test_size):
            if random.random() < 0.8:
                random_word = random.choice(word_list)
                prefix_len = random.randint(1, len(random_word) - 1)
                f.write(f"{random_word[:prefix_len]}\n")
            else:
                f.write(f"{''.join(random.choices(string.ascii_lowercase, k=3))}\n")

if __name__ == "__main__":
    generate_test_data()
    print("generisani dictionary.txt i test.txt!")