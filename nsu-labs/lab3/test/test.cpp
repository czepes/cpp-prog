#include "../src/config-parser.h"
#include "../src/converter.h"
#include "../src/errors.h"
#include "../src/soundp.h"
#include "../src/wav.h"
#include <cmath>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

namespace fs = std::filesystem;

// ==================== Helper Functions ====================

class SoundProcessorTest : public testing::Test {
protected:
  void SetUp() override {
    ConverterFactory::init();

    test_dir = fs::temp_directory_path() /
               ("soundp_test_" + std::to_string(time(nullptr)));
    fs::create_directory(test_dir);

    createTestWavFiles();
  }

  void TearDown() override { fs::remove_all(test_dir); }

  void createTestWavFiles() {
    std::vector<std::string> files = {"test1.wav", "test2.wav", "test3.wav"};

    for (const auto &filename : files) {
      std::string filepath = (test_dir / filename).string();

      auto writer = std::make_shared<WavWriter>(filepath);

      const int sampleRate = 44100;
      const double frequency = 440.0;
      const double amplitude = 10000.0;

      for (int i = 0; i < sampleRate; i++) {
        double t = static_cast<double>(i) / sampleRate;
        int16_t sample =
            static_cast<int16_t>(amplitude * sin(2 * M_PI * frequency * t));
        writer->write(sample);
      }

      writer->close();
    }
  }

  void createConfigFile(const std::string &filename,
                        const std::string &content) {
    std::ofstream configFile(test_dir / filename);
    configFile << content;
    configFile.close();
  }

  fs::path test_dir;
};

// ==================== Converter Tests ====================

TEST_F(SoundProcessorTest, MuterBasicTest) {

  std::string config_file = (test_dir / "config.txt").string();
  createConfigFile("config.txt", "mute 0.2 0.5");

  std::string input_file = (test_dir / "test1.wav").string();
  std::string output_file = (test_dir / "output.wav").string();

  std::vector<std::string> files = {output_file, input_file};

  SoundProcessor processor(config_file, files);
  ASSERT_NO_THROW({ processor.process(); })
      << "processor.process() threw an exception";
  EXPECT_TRUE(fs::exists(output_file))
      << "Output file not created: " << output_file;
  if (fs::exists(output_file)) {
    EXPECT_GT(fs::file_size(output_file), 0) << "Output file is empty";
  }
}

TEST_F(SoundProcessorTest, EmptyConfigFile) {
  std::string config_file = (test_dir / "config.txt").string();
  createConfigFile("config.txt", "# Пустой конфигурационный файл\n"
                                 "\n"
                                 "# Только комментарии\n");

  std::string input_file = (test_dir / "test1.wav").string();
  std::string output_file = (test_dir / "output.wav").string();

  std::vector<std::string> files = {output_file, input_file};

  SoundProcessor processor(config_file, files);
  ASSERT_NO_THROW(processor.process());

  EXPECT_TRUE(fs::exists(output_file));
  if (fs::exists(output_file)) {
    EXPECT_GT(fs::file_size(output_file), 0);
  }
}

TEST_F(SoundProcessorTest, MuterInvalidParameters) {
  createConfigFile("config.txt", "mute hello world");

  std::vector<std::string> files = {(test_dir / "output.wav").string(),
                                    (test_dir / "test1.wav").string()};

  SoundProcessor processor((test_dir / "config.txt").string(), files);
  EXPECT_THROW(processor.process(), std::runtime_error);
}

TEST_F(SoundProcessorTest, MuterStartGreaterThanEnd) {
  createConfigFile("config.txt", "mute 0.5 0.2");

  std::vector<std::string> files = {(test_dir / "output.wav").string(),
                                    (test_dir / "test1.wav").string()};

  SoundProcessor processor((test_dir / "config.txt").string(), files);
  EXPECT_THROW(processor.process(), std::runtime_error);
}

TEST_F(SoundProcessorTest, GainerBasicTest) {
  createConfigFile("config.txt", "gain 2.0 0.1 0.4");

  std::vector<std::string> files = {(test_dir / "output.wav").string(),
                                    (test_dir / "test1.wav").string()};

  SoundProcessor processor((test_dir / "config.txt").string(), files);
  ASSERT_NO_THROW(processor.process());
}

TEST_F(SoundProcessorTest, GainerInvalidFactor) {
  createConfigFile("config.txt", "gain -1.0");

  std::vector<std::string> files = {(test_dir / "output.wav").string(),
                                    (test_dir / "test1.wav").string()};

  SoundProcessor processor((test_dir / "config.txt").string(), files);
  EXPECT_THROW(processor.process(), std::runtime_error);
}

TEST_F(SoundProcessorTest, MixerBasicTest) {
  createConfigFile("config.txt", "mix $2 0.2 0.6");

  std::vector<std::string> files = {(test_dir / "output.wav").string(),
                                    (test_dir / "test1.wav").string(),
                                    (test_dir / "test2.wav").string()};

  SoundProcessor processor((test_dir / "config.txt").string(), files);
  ASSERT_NO_THROW(processor.process());
}

TEST_F(SoundProcessorTest, MixerInvalidFileReference) {
  createConfigFile("config.txt", "mix $5 0.2 0.6");

  std::vector<std::string> files = {(test_dir / "output.wav").string(),
                                    (test_dir / "test1.wav").string()};

  SoundProcessor processor((test_dir / "config.txt").string(), files);
  EXPECT_THROW(processor.process(), std::runtime_error);
}

TEST_F(SoundProcessorTest, CropperBasicTest) {
  createConfigFile("config.txt", "crop 0.2 0.7");

  std::vector<std::string> files = {(test_dir / "output.wav").string(),
                                    (test_dir / "test1.wav").string()};

  SoundProcessor processor((test_dir / "config.txt").string(), files);
  ASSERT_NO_THROW(processor.process());
}

TEST_F(SoundProcessorTest, CropperInvalidTimeRange) {
  createConfigFile("config.txt", "crop 1.5 0.5");

  std::vector<std::string> files = {(test_dir / "output.wav").string(),
                                    (test_dir / "test1.wav").string()};

  SoundProcessor processor((test_dir / "config.txt").string(), files);
  EXPECT_THROW(processor.process(), std::runtime_error);
}

// ==================== Multiple Commands Tests ====================

TEST_F(SoundProcessorTest, MultipleCommands) {
  createConfigFile("config.txt", "# Тестирование нескольких команд\n"
                                 "mute 0.1 0.3\n"
                                 "gain 1.5 0.4 0.8\n"
                                 "crop 0.2 0.9\n");

  std::vector<std::string> files = {(test_dir / "output.wav").string(),
                                    (test_dir / "test1.wav").string(),
                                    (test_dir / "test2.wav").string()};

  SoundProcessor processor((test_dir / "config.txt").string(), files);
  ASSERT_NO_THROW(processor.process());
}

TEST_F(SoundProcessorTest, CommentsAndEmptyLines) {
  createConfigFile("config.txt", "# Конфигурационный файл\n"
                                 "\n"
                                 "mute 0.1 0.3  # Комментарий в строке\n"
                                 "\n"
                                 "# Еще комментарий\n"
                                 "gain 2.0\n");

  std::vector<std::string> files = {(test_dir / "output.wav").string(),
                                    (test_dir / "test1.wav").string()};

  SoundProcessor processor((test_dir / "config.txt").string(), files);
  ASSERT_NO_THROW(processor.process());
}

// ==================== Edge Cases Tests ====================

TEST_F(SoundProcessorTest, InvalidConfigFile) {
  std::vector<std::string> files = {(test_dir / "output.wav").string(),
                                    (test_dir / "test1.wav").string()};

  EXPECT_THROW(SoundProcessor("nonexistent.txt", files), std::runtime_error);
}

TEST_F(SoundProcessorTest, InsufficientFiles) {
  createConfigFile("config.txt", "mute 0.1 0.3");

  std::vector<std::string> files = {(test_dir / "output.wav").string()};

  EXPECT_THROW(SoundProcessor((test_dir / "config.txt").string(), files),
               std::runtime_error);
}

TEST_F(SoundProcessorTest, InvalidConverterCommand) {
  createConfigFile("config.txt", "unknown_command param1 param2");

  std::vector<std::string> files = {(test_dir / "output.wav").string(),
                                    (test_dir / "test1.wav").string()};

  SoundProcessor processor((test_dir / "config.txt").string(), files);
  EXPECT_THROW(processor.process(), std::runtime_error);
}

// ==================== Converter Unit Tests ====================

class ConverterUnitTest : public ::testing::Test {
protected:
  void SetUp() override {
    ConverterFactory::init();

    test_dir = fs::temp_directory_path() /
               ("converter_test_" + std::to_string(time(nullptr)));
    fs::create_directory(test_dir);

    createTestWavFile("input.wav");
  }

  void TearDown() override { fs::remove_all(test_dir); }

  void createTestWavFile(const std::string &filename) {
    std::string filepath = (test_dir / filename).string();
    auto writer = std::make_shared<WavWriter>(filepath);

    const int sampleRate = 44100;
    const double frequency = 440.0;
    const double amplitude = 10000.0;

    for (int i = 0; i < sampleRate; i++) {
      double t = static_cast<double>(i) / sampleRate;
      int16_t sample;

      if (t < 0.5) {
        sample = 0;
      } else {
        sample =
            static_cast<int16_t>(amplitude * sin(2 * M_PI * frequency * t));
      }
      writer->write(sample);
    }

    writer->close();
  }

  fs::path test_dir;
};

TEST_F(ConverterUnitTest, MuterConverterTest) {
  auto input = std::make_shared<WavReader>((test_dir / "input.wav").string());
  auto output = std::make_shared<WavWriter>((test_dir / "output.wav").string());

  std::vector<std::string> params = {"0.1", "0.3"};
  auto converter = ConverterFactory::create("mute", output, input, params, 1);

  ASSERT_NO_THROW(converter->convert());
  output->close();

  EXPECT_TRUE(fs::exists(test_dir / "output.wav"));

  auto outputReader =
      std::make_shared<WavReader>((test_dir / "output.wav").string());
  EXPECT_EQ(outputReader->get_total_samples(), input->get_total_samples());
}

TEST_F(ConverterUnitTest, GainerConverterTest) {
  auto input = std::make_shared<WavReader>((test_dir / "input.wav").string());
  auto output = std::make_shared<WavWriter>((test_dir / "output.wav").string());

  std::vector<std::string> params = {"2.0", "0.5", "1.0"};
  auto converter = ConverterFactory::create("gain", output, input, params, 1);

  ASSERT_NO_THROW(converter->convert());
  output->close();

  EXPECT_TRUE(fs::exists(test_dir / "output.wav"));
}

TEST_F(ConverterUnitTest, MixerConverterTest) {
  createTestWavFile("input2.wav");

  auto input = std::make_shared<WavReader>((test_dir / "input.wav").string());
  auto output = std::make_shared<WavWriter>((test_dir / "output.wav").string());

  std::vector<std::string> params = {(test_dir / "input2.wav").string(), "0.2",
                                     "0.8"};
  auto converter = ConverterFactory::create("mix", output, input, params, 1);

  ASSERT_NO_THROW(converter->convert());
  output->close();

  EXPECT_TRUE(fs::exists(test_dir / "output.wav"));
}

TEST_F(ConverterUnitTest, CropperConverterTest) {
  auto input = std::make_shared<WavReader>((test_dir / "input.wav").string());
  auto output = std::make_shared<WavWriter>((test_dir / "output.wav").string());

  std::vector<std::string> params = {"0.3", "0.7"};
  auto converter = ConverterFactory::create("crop", output, input, params, 1);

  ASSERT_NO_THROW(converter->convert());
  output->close();

  EXPECT_TRUE(fs::exists(test_dir / "output.wav"));

  auto outputReader =
      std::make_shared<WavReader>((test_dir / "output.wav").string());
  EXPECT_LT(outputReader->get_total_samples(), input->get_total_samples());
}

// ==================== Config Parser Tests ====================

TEST(ConfigParserTest, ParseTimeParamValid) {
  EXPECT_DOUBLE_EQ(ConfigParser::parse_time_param("0.5", 10.0, 1), 0.5);
  EXPECT_DOUBLE_EQ(ConfigParser::parse_time_param("-1", 10.0, 1, true), -1.0);
  EXPECT_DOUBLE_EQ(ConfigParser::parse_time_param("0", 10.0, 1), 0.0);
}

TEST(ConfigParserTest, ParseTimeParamInvalid) {
  EXPECT_THROW(ConfigParser::parse_time_param("invalid", 10.0, 1),
               std::runtime_error);
  EXPECT_THROW(ConfigParser::parse_time_param("11.0", 10.0, 1),
               std::runtime_error);
  EXPECT_THROW(ConfigParser::parse_time_param("-2", 10.0, 1),
               std::runtime_error);
}

TEST(ConfigParserTest, ParseFactorParamValid) {
  EXPECT_DOUBLE_EQ(ConfigParser::parse_factor_param("1.5", 0.0, 10.0, 1), 1.5);
  EXPECT_DOUBLE_EQ(ConfigParser::parse_factor_param("0.0", 0.0, 10.0, 1), 0.0);
  EXPECT_DOUBLE_EQ(ConfigParser::parse_factor_param("10.0", 0.0, 10.0, 1),
                   10.0);
}

TEST(ConfigParserTest, ParseFactorParamInvalid) {
  EXPECT_THROW(ConfigParser::parse_factor_param("-1.0", 0.0, 10.0, 1),
               std::runtime_error);
  EXPECT_THROW(ConfigParser::parse_factor_param("11.0", 0.0, 10.0, 1),
               std::runtime_error);
  EXPECT_THROW(ConfigParser::parse_factor_param("invalid", 0.0, 10.0, 1),
               std::runtime_error);
}

// ==================== Converter Factory Tests ====================

TEST(ConverterFactoryTest, HasConverter) {
  ConverterFactory::init();

  EXPECT_TRUE(ConverterFactory::help().find("mute") != std::string::npos);
  EXPECT_TRUE(ConverterFactory::help().find("gain") != std::string::npos);
  EXPECT_TRUE(ConverterFactory::help().find("mix") != std::string::npos);
  EXPECT_TRUE(ConverterFactory::help().find("crop") != std::string::npos);
}

TEST(ConverterFactoryTest, UnknownConverter) {
  ConverterFactory::init();

  EXPECT_FALSE(ConverterFactory::help().empty());

  EXPECT_TRUE(ConverterFactory::help("mute").find("Unknown converter") ==
              std::string::npos);
  EXPECT_TRUE(ConverterFactory::help("gain").find("Unknown converter") ==
              std::string::npos);
  EXPECT_TRUE(ConverterFactory::help("mix").find("Unknown converter") ==
              std::string::npos);
  EXPECT_TRUE(ConverterFactory::help("crop").find("Unknown converter") ==
              std::string::npos);

  EXPECT_TRUE(ConverterFactory::help("nonexistent").find("Unknown converter") !=
              std::string::npos);
}

class WavFileTest : public testing::Test {
protected:
  void SetUp() override {
    test_dir = fs::temp_directory_path() /
               ("wav_test_" + std::to_string(time(nullptr)));
    fs::create_directory(test_dir);

    createValidWavFile();
    createInvalidWavFile();
    createEmptyWavFile();
  }

  void TearDown() override { fs::remove_all(test_dir); }

  void createValidWavFile() {
    std::string filepath = (test_dir / "valid.wav").string();
    auto writer = std::make_shared<WavWriter>(filepath);

    const int sampleRate = 44100;
    const double frequency = 440.0;
    const double amplitude = 10000.0;

    for (int i = 0; i < sampleRate; i++) {
      double t = static_cast<double>(i) / sampleRate;
      int16_t sample =
          static_cast<int16_t>(amplitude * sin(2 * M_PI * frequency * t));
      writer->write(sample);
    }

    writer->close();
  }

  void createInvalidWavFile() {
    std::string filepath = (test_dir / "invalid.wav").string();
    std::ofstream file(filepath, std::ios::binary);

    char invalid_header[44] = {0};
    file.write(invalid_header, 44);
    file.close();
  }

  void createEmptyWavFile() {
    std::string filepath = (test_dir / "empty.wav").string();
    auto writer = std::make_shared<WavWriter>(filepath);
    writer->close();
  }

  fs::path test_dir;
};

TEST_F(WavFileTest, WavReaderValidFile) {
  std::string valid_file = (test_dir / "valid.wav").string();

  EXPECT_NO_THROW({
    WavReader reader(valid_file);

    EXPECT_EQ(reader.get_header().sample_rate, 44100);
    EXPECT_EQ(reader.get_header().num_channels, 1);
    EXPECT_EQ(reader.get_header().bits_per_sample, 16);
    EXPECT_GT(reader.get_total_samples(), 0);

    EXPECT_GT(reader.get_duration(), 0.0);
    EXPECT_EQ(reader.get_remaining_samples(), reader.get_total_samples());

    EXPECT_EQ(reader.seconds_to_sample(1.0), 44100);
    EXPECT_DOUBLE_EQ(reader.sample_to_seconds(44100), 1.0);
  });
}

TEST_F(WavFileTest, WavReaderInvalidFile) {
  std::string invalid_file = (test_dir / "invalid.wav").string();

  EXPECT_THROW({ WavReader reader(invalid_file); }, WavError);
}

TEST_F(WavFileTest, WavReaderNonExistentFile) {
  EXPECT_THROW({ WavReader reader("nonexistent.wav"); }, WavError);
}

TEST_F(WavFileTest, WavReaderReadOperations) {
  std::string valid_file = (test_dir / "valid.wav").string();
  WavReader reader(valid_file);

  audio_buffer_t samples;
  size_t num_samples = 1000;
  size_t read_count = reader.read(samples, num_samples);

  EXPECT_EQ(read_count, num_samples);
  EXPECT_EQ(samples.size(), num_samples);
  EXPECT_EQ(reader.get_current_pos(), num_samples);

  samples.clear();
  read_count = reader.read(samples, 0.1, 0.2);
  size_t expected_samples = static_cast<size_t>(0.1 * 44100);
  EXPECT_GT(read_count, 0);
  EXPECT_EQ(read_count, expected_samples);

  size_t current_pos = reader.get_current_pos();
  reader.skip(500);
  EXPECT_EQ(reader.get_current_pos(), current_pos + 500);

  reader.reset();
  EXPECT_EQ(reader.get_current_pos(), 0);

  samples.clear();
  read_count = reader.read(samples);
  EXPECT_EQ(read_count, reader.get_total_samples());
  EXPECT_TRUE(reader.eof());
}

TEST_F(WavFileTest, WavReaderSkipTo) {
  std::string valid_file = (test_dir / "valid.wav").string();
  WavReader reader(valid_file);

  size_t target_sample = 22050;
  reader.skip_to(target_sample);

  EXPECT_EQ(reader.get_current_pos(), target_sample);
  EXPECT_DOUBLE_EQ(reader.sample_to_seconds(target_sample), 0.5);

  reader.skip_to(target_sample);
  EXPECT_EQ(reader.get_current_pos(), target_sample);
}

TEST_F(WavFileTest, WavWriterBasicOperations) {
  std::string output_file = (test_dir / "output.wav").string();

  {
    WavWriter writer(output_file);

    writer.write(1000);
    EXPECT_EQ(writer.get_current_pos(), 1);

    audio_buffer_t samples = {2000, 3000, 4000};
    writer.write(samples);
    EXPECT_EQ(writer.get_current_pos(), 4);

    writer.write_silence(100);
    EXPECT_EQ(writer.get_current_pos(), 104);

    writer.close();
  }

  EXPECT_TRUE(fs::exists(output_file));

  WavReader reader(output_file);
  EXPECT_EQ(reader.get_total_samples(), 104);
}

TEST_F(WavFileTest, WavWriterEmptyFile) {
  std::string empty_file = (test_dir / "empty_output.wav").string();

  {
    WavWriter writer(empty_file);
    writer.close();
  }

  EXPECT_TRUE(fs::exists(empty_file));

  WavReader reader(empty_file);
  EXPECT_EQ(reader.get_total_samples(), 0);
  EXPECT_DOUBLE_EQ(reader.get_duration(), 0.0);
}

TEST_F(WavFileTest, WavWriterReopenSameFile) {
  std::string filepath = (test_dir / "reopen.wav").string();

  {
    WavWriter writer1(filepath);
    writer1.write(1000);
    writer1.close();
  }

  {
    WavWriter writer2(filepath);
    writer2.write(2000);
    writer2.close();
  }

  WavReader reader(filepath);
  EXPECT_EQ(reader.get_total_samples(), 1);
}

TEST_F(WavFileTest, WavWriterWriteToClosedStream) {
  std::string filepath = (test_dir / "closed.wav").string();
  WavWriter writer(filepath);
  writer.close();

  EXPECT_THROW({ writer.write(1000); }, WavError);

  audio_buffer_t samples = {1000, 2000};
  EXPECT_THROW({ writer.write(samples); }, WavError);

  EXPECT_THROW({ writer.write_silence(10); }, WavError);
}

TEST_F(WavFileTest, WavReaderReadFromClosedStream) {
  std::string filepath = (test_dir / "closed_reader.wav").string();
  WavWriter writer(filepath);
  writer.write(1000);
  writer.close();

  WavReader reader(filepath);
  reader.close();

  audio_buffer_t samples;
  EXPECT_THROW({ reader.read(samples, 10); }, WavError);
}

TEST_F(WavFileTest, WavHeaderValidation) {
  WavHeader valid_header;
  EXPECT_TRUE(WavFile::validate_header(valid_header));

  WavHeader invalid_header;

  invalid_header = valid_header;
  invalid_header.riff[0] = 'X';
  EXPECT_FALSE(WavFile::validate_header(invalid_header));

  invalid_header = valid_header;
  invalid_header.wave[0] = 'X';
  EXPECT_FALSE(WavFile::validate_header(invalid_header));

  invalid_header = valid_header;
  invalid_header.audio_format = 2;
  EXPECT_FALSE(WavFile::validate_header(invalid_header));

  invalid_header = valid_header;
  invalid_header.num_channels = 3;
  EXPECT_FALSE(WavFile::validate_header(invalid_header));

  invalid_header = valid_header;
  invalid_header.bits_per_sample = 8;
  EXPECT_FALSE(WavFile::validate_header(invalid_header));
}

TEST_F(WavFileTest, WavReaderReadInvalidTimeRange) {
  std::string valid_file = (test_dir / "valid.wav").string();
  WavReader reader(valid_file);

  audio_buffer_t samples;

  size_t read_count = reader.read(samples, 0.5, 0.2);
  EXPECT_EQ(read_count, 0);
  EXPECT_TRUE(samples.empty());

  double duration = reader.get_duration();
  read_count = reader.read(samples, duration + 1.0, duration + 2.0);
  EXPECT_EQ(read_count, 0);
  EXPECT_TRUE(samples.empty());

  reader.reset();
  read_count = reader.read(samples, 0.0, duration + 10.0);
  EXPECT_EQ(read_count, reader.get_total_samples());
}

// ==================== Errors Tests ====================

TEST(ErrorsTest, ConfigParserError) {
  try {
    throw ConfigParserError("Config error");
    FAIL() << "Should have thrown exception";
  } catch (const ConfigParserError &e) {
    EXPECT_EQ(e.code(), CONFIG_PARSER_ERROR_CODE);
  }

  try {
    throw ConfigParserError(10, "Config line error");
    FAIL() << "Should have thrown exception";
  } catch (const ConfigParserError &e) {
    EXPECT_EQ(e.code(), CONFIG_PARSER_ERROR_CODE);
    EXPECT_EQ(e.get_line_number(), 10);
  }
}

TEST(ErrorsTest, ConverterError) {
  try {
    throw ConverterError("Converter error");
    FAIL() << "Should have thrown exception";
  } catch (const ConverterError &e) {
    EXPECT_EQ(e.code(), CONVERTER_ERROR_CODE);
  }

  try {
    throw ConverterError(20, "Converter line error");
    FAIL() << "Should have thrown exception";
  } catch (const ConverterError &e) {
    EXPECT_EQ(e.code(), CONVERTER_ERROR_CODE);
    EXPECT_EQ(e.get_line_number(), 20);
  }
}

TEST(ErrorsTest, SoundProcessorError) {
  try {
    throw SoundProcessorError("Processor error");
    FAIL() << "Should have thrown exception";
  } catch (const SoundProcessorError &e) {
    EXPECT_EQ(e.code(), SOUND_PROCESSOR_ERROR_CODE);
    EXPECT_NE(std::string(e.what()).find("Processor error"), std::string::npos);
  }
}

TEST(ErrorsTest, WavError) {
  try {
    throw WavError("WAV file error");
    FAIL() << "Should have thrown exception";
  } catch (const WavError &e) {
    EXPECT_EQ(e.code(), WAV_ERROR_CODE);
    EXPECT_NE(std::string(e.what()).find("WAV file error"), std::string::npos);
  }
}

TEST(ErrorsTest, ArgParserError) {
  try {
    throw ArgParserError("Argument parser error");
    FAIL() << "Should have thrown exception";
  } catch (const ArgParserError &e) {
    EXPECT_EQ(e.code(), ARGPARSER_ERROR_CODE);
    EXPECT_NE(std::string(e.what()).find("Argument parser error"),
              std::string::npos);
  }
}

TEST(ErrorsTest, ExceptionHierarchy) {
  std::vector<std::unique_ptr<SoundError>> errors;

  errors.push_back(std::make_unique<ConfigParserError>("Config"));
  errors.push_back(std::make_unique<ConverterError>("Converter"));
  errors.push_back(std::make_unique<SoundProcessorError>("Processor"));
  errors.push_back(std::make_unique<WavError>("WAV"));
  errors.push_back(std::make_unique<ArgParserError>("ArgParser"));

  for (const auto &error : errors) {
    EXPECT_NE(error->what(), nullptr);
    EXPECT_GT(error->code(), 0);
  }

  SoundLineError *line_error = new ConfigParserError(1, "Test");
  EXPECT_EQ(line_error->get_line_number(), 1);
  delete line_error;
}

// ==================== WavReader Edge Cases ====================

TEST_F(WavFileTest, WavReaderEmptyBufferRead) {
  std::string valid_file = (test_dir / "valid.wav").string();
  WavReader reader(valid_file);

  audio_buffer_t samples;
  size_t read_count = reader.read(samples, 0);
  EXPECT_EQ(read_count, 0);
  EXPECT_TRUE(samples.empty());

  reader.skip_to(reader.get_total_samples());
  read_count = reader.read(samples, 100);
  EXPECT_EQ(read_count, 0);
}

TEST_F(WavFileTest, WavReaderReadLargeBuffer) {
  std::string valid_file = (test_dir / "valid.wav").string();
  WavReader reader(valid_file);

  audio_buffer_t samples;
  size_t total_samples = reader.get_total_samples();
  size_t read_count = reader.read(samples, total_samples * 2);

  EXPECT_EQ(read_count, total_samples);
  EXPECT_EQ(samples.size(), total_samples);
  EXPECT_TRUE(reader.eof());
}

TEST_F(WavFileTest, WavWriterHeaderUpdate) {
  std::string filepath = (test_dir / "header_test.wav").string();

  {
    WavWriter writer(filepath);

    for (int i = 0; i < 1000; i++) {
      writer.write(static_cast<sample_t>(i % 1000));
    }

    writer.close();
  }

  WavReader reader(filepath);
  EXPECT_EQ(reader.get_total_samples(), 1000);
  EXPECT_EQ(reader.get_header().data_size, 1000 * sizeof(sample_t));
}

// ==================== Integration Tests ====================

TEST_F(WavFileTest, WavReadWriteConsistency) {
  std::string input_file = (test_dir / "input.wav").string();
  std::string output_file = (test_dir / "output.wav").string();

  {
    WavWriter writer(input_file);
    for (int i = 0; i < 1000; i++) {
      writer.write(static_cast<sample_t>(i - 500));
    }
    writer.close();
  }

  {
    WavReader reader(input_file);
    WavWriter writer(output_file);

    audio_buffer_t samples;

    while (!reader.eof()) {
      size_t read_count = reader.read(samples, 100);
      if (read_count > 0) {
        writer.write(samples);
      }
    }

    reader.close();
    writer.close();
  }

  WavReader input_reader(input_file);
  WavReader output_reader(output_file);

  EXPECT_EQ(input_reader.get_total_samples(),
            output_reader.get_total_samples());

  audio_buffer_t input_samples, output_samples;
  input_reader.read(input_samples);
  output_reader.read(output_samples);

  EXPECT_EQ(input_samples.size(), output_samples.size());

  for (size_t i = 0; i < input_samples.size(); i++) {
    EXPECT_EQ(input_samples[i], output_samples[i]);
  }
}

TEST_F(WavFileTest, WavReaderSeekAndRead) {
  std::string filepath = (test_dir / "seek_test.wav").string();

  {
    WavWriter writer(filepath);
    for (int i = 0; i < 100; i++) {
      writer.write(static_cast<sample_t>(i));
    }
    writer.close();
  }

  WavReader reader(filepath);

  audio_buffer_t samples;

  reader.skip_to(50);
  reader.read(samples, 10);
  EXPECT_EQ(samples.size(), 10);
  EXPECT_EQ(samples[0], 50);
  EXPECT_EQ(samples[9], 59);

  reader.reset();
  reader.skip(25);
  samples.clear();
  reader.read(samples, 10);
  EXPECT_EQ(samples.size(), 10);
  EXPECT_EQ(samples[0], 25);
  EXPECT_EQ(samples[9], 34);
}

// ==================== Main Function ====================

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
