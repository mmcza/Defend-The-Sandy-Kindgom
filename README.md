# Defend The Sandy Kindgom

Disclaimer: you need [SFML-2.5.1](https://www.sfml-dev.org/index.php "SFML website") on your computer to be able to compile the files.

Point of the game is to make it impossible for opponents to get to the left side of the map and protect the castle. Map is randomly generated at beginning of each game. Bullets trajectory is calculated using physics laws for projectile motion. There are different types of opponents that move with different speed (and some of them can fly), give different number of gold and points. The most common one is the soldier and most rare is the dragon.

<p allign="center">
  <img src="https://user-images.githubusercontent.com/46852756/221369644-91a83d23-4a2f-406d-a136-537ab87e34ff.png">
</p>

Player is starting with a cannon that has a limited number of bullets that can be simultaneously in the air and their task is to set proper angle. Hitting each enemy guarantees some amount of gold (that can be used to upgrade the cannon) and points (the only way to win the game is to get 4000 points). If hp gets to 0 game ends as a loss.
