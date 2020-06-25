#!/usr/bin/env python3
#
# Get a random quote from wikiquotes

import random
import wikiquotes

# Get quotes from these people
people = [
    # Science
    'Ada Lovelace',
    'Carl Sagan',
    'Charles Darwin',
    'Edith Clarke',
    'Grace Hopper',
    'Hedy Lamarr',
    'Isaac Newton',
    'James Clerk Maxwell',
    'Margaret Hamilton',
    'Marie Curie',
    'Michael Faraday',
    'Niels Bohr',
    'Nikola Tesla',
    'Rosalind Franklin',
    'Stephen Hawking',
    # Comedy
    'Bill Bryson',
    'Dave Barry',
    'Douglas Adams',
    'Eddie Izzard',
    'Jack Handey',
    'Mitch Hedberg',
    'Ryan North',
    'Sarah Silverman',
    # Atheism
    'Ayaan Hirsi Ali',
    'Christopher Hitchens',
    'David Hume',
    'David Silverman',
    'Lawrence M. Krauss',
    'Penn Jilette',
    'Richard Dawkins',
    'Sam Harris',
    # Software
    'Bjarne Stroustrup',
    'Linux Torvalds'
]

# Get between 3 and 5 random quotes
def main():
    # Seed the RNG   
    random.seed()
    
    # Get a random number of quotes
    print()
    for n in range(random.randint(3, 5)):
        random_quote()
        print()
    # for
# main()

# Get a random quote
def random_quote():
    # Pick a random quote from a random person
    person = random.choice(people)
    quote = wikiquotes.random_quote(person, 'english')
    print('%s\n  -- %s' % (quote, person))
# random_quote()

# Call the main function
main()
