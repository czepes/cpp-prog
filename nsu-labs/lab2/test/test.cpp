#include "../src/patterns.h"
#include "../src/simulator.h"
#include <gtest/gtest.h>
#include <stdexcept>

class GameTest : public testing::Test {
protected:
  Simulator *empty;
  Simulator *fixed;
  Simulator *sim;
  Simulator *fsim;
  Simulator *fssim;

  pair<int, int> size;

  GameTest() {
    empty = new Simulator();
    sim = new Simulator(true);
    fixed = new Simulator(pair(20, 20));

    ifstream in("../data/pulsar.lif");
    fsim = new Simulator(in);
    in.close();

    in.open("../data/beacon.lif");
    size = pair(6, 6);
    fssim = new Simulator(size, in);
    in.close();
  }

  ~GameTest() {
    delete empty;
    delete fixed;
    delete sim;
    delete fsim;
    delete fssim;
  }
};

TEST_F(GameTest, Construction) {
  ASSERT_EQ(sim->get_name(), "Conway's Game of Life");
  ASSERT_EQ(sim->get_birth_rule(), "3");
  ASSERT_EQ(sim->get_survival_rule(), "23");

  ASSERT_EQ(fsim->get_name(), "Pulsar");
  ASSERT_EQ(fsim->get_birth_rule(), "3");
  ASSERT_EQ(fsim->get_survival_rule(), "23");

  ASSERT_EQ(fssim->get_name(), "Beacon");
  ASSERT_EQ(fssim->get_birth_rule(), "3");
  ASSERT_EQ(fssim->get_survival_rule(), "23");
  ASSERT_EQ(fssim->get_cells().get_size(), size);

  Cells &cells{fssim->get_cells()};
  ASSERT_TRUE(cells[1][1]);
  ASSERT_TRUE(cells[1][2]);
  ASSERT_TRUE(cells[2][1]);
  ASSERT_TRUE(cells[3][4]);
  ASSERT_TRUE(cells[4][3]);
  ASSERT_TRUE(cells[4][4]);
}

TEST_F(GameTest, Parsing) {
  ifstream in("../data/wrong-name.lif");
  EXPECT_THROW(Simulator sim(in), invalid_argument);
  in.close();

  in.open("../data/wrong-format.lif");
  EXPECT_THROW(Simulator sim(in), invalid_argument);
  in.close();

  in.open("../data/missing-rules.lif");
  EXPECT_THROW(Simulator sim(in), invalid_argument);
  in.close();

  in.open("../data/missing-birth-rule.lif");
  EXPECT_THROW(Simulator sim(in), invalid_argument);
  in.close();

  in.open("../data/missing-survival-rule.lif");
  EXPECT_THROW(Simulator sim(in), invalid_argument);
  in.close();

  in.open("../data/missing-rule-sep.lif");
  EXPECT_THROW(Simulator sim(in), invalid_argument);
  in.close();

  in.open("../data/wrong-rule-values.lif");
  EXPECT_THROW(Simulator sim(in), invalid_argument);
  in.close();

  in.open("../data/wrong-rule-values-2.lif");
  EXPECT_THROW(Simulator sim(in), invalid_argument);
  in.close();

  in.open("../data/empty-rule.lif");
  EXPECT_THROW(Simulator sim(in), invalid_argument);
  in.close();

  in.open("../data/missing-rules-literals.lif");
  EXPECT_THROW(Simulator sim(in), invalid_argument);
  in.close();

  in.open("../data/unknown-rule.lif");
  EXPECT_THROW(Simulator sim(in), invalid_argument);
  in.close();

  in.open("../data/missing-cells.lif");
  EXPECT_THROW(Simulator sim(in), invalid_argument);
  in.close();

  in.open("../data/wrong-cells.lif");
  EXPECT_THROW(Simulator sim(in), invalid_argument);
  in.close();

  in.open("../data/wrong-cells-2.lif");
  EXPECT_THROW(Simulator sim(in), invalid_argument);
  in.close();

  in.open("../data/beacon.lif");
  EXPECT_NO_THROW(Simulator sim(in));
  in.close();
}

TEST_F(GameTest, Setters) {
  fixed->set_name("New Name");
  fixed->set_birth_rule("12345678");
  fixed->set_survival_rule("12345678");

  ASSERT_EQ(fixed->get_name(), "New Name");
  ASSERT_EQ(fixed->get_birth_rule(), "12345678");
  ASSERT_EQ(fixed->get_survival_rule(), "12345678");

  fixed->set_birth_rule("90");
  fixed->set_survival_rule("abc");

  ASSERT_NE(fixed->get_birth_rule(), "90");
  ASSERT_NE(fixed->get_survival_rule(), "abc");
  ASSERT_EQ(fixed->get_birth_rule(), "12345678");
  ASSERT_EQ(fixed->get_survival_rule(), "12345678");

  Cells &cells = fixed->get_cells();
  cells.clear();
  for (const auto row : cells) {
    for (const auto cell : row) {
      ASSERT_FALSE(cell);
    }
  }
}

TEST_F(GameTest, Simulating) {
  Cells &cells = fixed->get_cells();

  cells[0][0] = true;

  put_block(cells, -3, -3);

  cells[2][2] = true;
  cells[2][4] = true;
  cells[3][3] = true;
  cells[4][2] = true;
  cells[4][4] = true;

  EXPECT_TRUE(cells[0][0]);
  EXPECT_TRUE(find_block(cells, -3, -3));
  EXPECT_TRUE(cells[2][2] && cells[2][4] && cells[3][3] && cells[4][2] &&
              cells[4][4]);

  fixed->live(1);

  EXPECT_FALSE(cells[0][0]);
  EXPECT_TRUE(find_block(cells, -3, -3));
  EXPECT_FALSE(cells[2][2] || cells[2][4] || cells[3][3] || cells[4][2] ||
               cells[4][4]);
  EXPECT_TRUE(cells[2][3] && cells[3][2] && cells[3][4] && cells[4][3]);

  fixed->live(10);

  EXPECT_TRUE(find_block(cells, -3, -3));
  EXPECT_TRUE(cells[2][3] && cells[3][2] && cells[3][4] && cells[4][3]);
}

TEST_F(GameTest, NoSurvival) {
  fixed->set_survival_rule("");
  fixed->set_birth_rule("");
  Cells &cells = fixed->get_cells();

  cells[0][0] = true;

  put_block(cells, -3, -3);

  cells[2][2] = true;
  cells[2][4] = true;
  cells[3][3] = true;
  cells[4][2] = true;
  cells[4][4] = true;

  EXPECT_TRUE(cells[0][0]);
  EXPECT_TRUE(find_block(cells, -3, -3));
  EXPECT_TRUE(cells[2][2] && cells[2][4] && cells[3][3] && cells[4][2] &&
              cells[4][4]);

  fixed->live(1);

  for (const auto &row : cells) {
    for (const auto &cell : row) {
      EXPECT_FALSE(cell);
    }
  }
}

TEST_F(GameTest, NoDying) {
  fixed->set_survival_rule("12345678");
  fixed->set_birth_rule("12345678");
  Cells &cells = fixed->get_cells();
  int height{cells.get_size().first};
  int width{cells.get_size().second};

  put_block(cells, 0, 0, height, width, false);
  EXPECT_TRUE(find_block(cells, 0, 0, height, width, false));

  fixed->live(height);

  for (const auto row : cells) {
    for (const auto cell : row) {
      EXPECT_TRUE(cell);
    }
  }
}

TEST_F(GameTest, CustomRules) {
  fixed->set_survival_rule("8");
  fixed->set_birth_rule("");
  Cells &cells = fixed->get_cells();

  put_block(cells, 0, 0, 3, 3);

  EXPECT_TRUE(find_block(cells, 0, 0, 3, 3));

  fixed->live();

  EXPECT_FALSE(find_block(cells, 0, 0, 3, 3));
  EXPECT_TRUE(cells[1][1]);

  fixed->live();

  EXPECT_FALSE(cells[1][1]);
}

TEST_F(GameTest, DumpLoad) {
  string new_name = "New Name";
  string new_rule = "3";
  Cells &cells = fssim->get_cells();

  fssim->set_name(new_name);
  fssim->set_survival_rule(new_rule);

  fssim->live();

  EXPECT_TRUE(cells[2][2]);
  EXPECT_TRUE(cells[3][3]);

  ofstream out;
  out.open("temp");
  (*fssim) >> out;
  out.close();

  ifstream in;
  in.open("../data/beacon.lif");
  (*fssim) << in;
  in.close();

  EXPECT_NE(fssim->get_name(), new_name);
  EXPECT_NE(fssim->get_survival_rule(), new_rule);

  EXPECT_FALSE(cells[2][2]);
  EXPECT_FALSE(cells[3][3]);

  in.open("temp");
  (*fssim) << in;
  in.close();

  EXPECT_EQ(fssim->get_name(), new_name);
  EXPECT_EQ(fssim->get_survival_rule(), new_rule);

  EXPECT_TRUE(cells[2][2]);
  EXPECT_TRUE(cells[3][3]);

  (*fssim) << in;
  (*fssim) >> out;

  system("rm temp");
}
