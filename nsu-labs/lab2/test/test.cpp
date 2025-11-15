#include "../src/patterns.h"
#include "../src/simulator.h"
#include <gtest/gtest.h>
#include <stdexcept>

class GameTest : public testing::Test {
protected:
  Simulator *cln;
  Simulator *sim;
  Simulator *fsim;
  Simulator *fssim;

  pair<int, int> size;

  GameTest() {
    cln = new Simulator(pair(20, 20));
    sim = new Simulator();

    ifstream in("../data/pulsar.lif");
    fsim = new Simulator(in);
    in.close();

    in.open("../data/beacon.lif");
    size = pair(6, 6);
    fssim = new Simulator(size, in);
    in.close();
  }

  ~GameTest() {
    delete cln;
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
  cln->set_name("New Name");
  cln->set_birth_rule("12345678");
  cln->set_survival_rule("12345678");

  ASSERT_EQ(cln->get_name(), "New Name");
  ASSERT_EQ(cln->get_birth_rule(), "12345678");
  ASSERT_EQ(cln->get_survival_rule(), "12345678");

  cln->set_birth_rule("90");
  cln->set_survival_rule("abc");

  ASSERT_NE(cln->get_birth_rule(), "90");
  ASSERT_NE(cln->get_survival_rule(), "abc");
  ASSERT_EQ(cln->get_birth_rule(), "12345678");
  ASSERT_EQ(cln->get_survival_rule(), "12345678");
}

TEST_F(GameTest, Simulating) {
  Cells &cells = cln->get_cells();

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

  cln->live(1);

  EXPECT_FALSE(cells[0][0]);
  EXPECT_TRUE(find_block(cells, -3, -3));
  EXPECT_FALSE(cells[2][2] || cells[2][4] || cells[3][3] || cells[4][2] ||
               cells[4][4]);
  EXPECT_TRUE(cells[2][3] && cells[3][2] && cells[3][4] && cells[4][3]);

  cln->live(10);

  EXPECT_TRUE(find_block(cells, -3, -3));
  EXPECT_TRUE(cells[2][3] && cells[3][2] && cells[3][4] && cells[4][3]);
}

TEST_F(GameTest, NoSurvival) {
  cln->set_survival_rule("");
  cln->set_birth_rule("");
  Cells &cells = cln->get_cells();

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

  cln->live(1);

  for (const auto &row : cells) {
    for (const auto &cell : row) {
      EXPECT_FALSE(cell);
    }
  }
}

TEST_F(GameTest, NoDying) {
  cln->set_survival_rule("12345678");
  cln->set_birth_rule("12345678");
  Cells &cells = cln->get_cells();
  int height{cells.get_size().first};
  int width{cells.get_size().second};

  put_block(cells, 0, 0, height, width, false);
  EXPECT_TRUE(find_block(cells, 0, 0, height, width, false));

  cln->live(height);

  for (const auto row : cells) {
    for (const auto cell : row) {
      EXPECT_TRUE(cell);
    }
  }
}

TEST_F(GameTest, CustomRules) {
  cln->set_survival_rule("8");
  cln->set_birth_rule("");
  Cells &cells = cln->get_cells();

  put_block(cells, 0, 0, 3, 3);

  EXPECT_TRUE(find_block(cells, 0, 0, 3, 3));

  cln->live();

  EXPECT_FALSE(find_block(cells, 0, 0, 3, 3));
  EXPECT_TRUE(cells[1][1]);

  cln->live();

  EXPECT_FALSE(cells[1][1]);
}
