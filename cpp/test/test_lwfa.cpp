#include "../src/lwfa.hpp"
#include "../src/ini.hpp"
#include "../src/score.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <algorithm>
#include <chrono>

std::vector<int32_t> get_python_wavefront_score(std::string file_name)
{
    std::ifstream fin(file_name);
    if (!fin.is_open())
    {
        std::exit(1);
    }
    std::string line;
    std::getline(fin, line);

    std::vector<int32_t> s_ts;
    while (std::getline(fin, line))
    {
        std::stringstream ss(line);
        std::string s_t;
        std::getline(ss, s_t, ',');
        s_ts.push_back(std::stoi(s_t));
    }

    return s_ts;
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::exit(1);
    }
    auto cfg = read_ini(argv[1]);

    double total_time = 0;
    for (const auto score_schema_name_ : cfg["score_schema"])
    {
        std::string score_schema_name = std::get<std::string>(score_schema_name_);
        ScoreCanonical score_schema;
        if (score_schema_name == "bowtie")
        {
            score_schema = bowtie_score;
        }
        else
        {
            // score_schema_name == "bwa"
            score_schema = bwa_score;
        }
        auto [l_c, x_c, o_c, e_c] = score_schema;
        auto [x, o, e, j] = score_system_canonical2wavefront(l_c, x_c, o_c, e_c);

        for (const auto end_up_ : cfg["end_up"])
        {
            int32_t end_up = std::get<int>(end_up_);
            for (auto mode_ : cfg["mode"])
            {
                std::string mode = std::get<std::string>(mode_);
                char c_f, c_r;
                if (mode == "global")
                {
                    c_f = 'M';
                    c_r = 'M';
                }
                else
                {
                    // mode == "local"
                    c_f = 'L';
                    c_r = 'L';
                }
                for (auto length_ : cfg["length"])
                {
                    int32_t length = std::get<int>(length_);
                    for (auto err_ : cfg["err"])
                    {
                        std::string err = std::get<std::string>(err_);

                        std::string python_file = "paper/test/align/" + score_schema_name + '/' + std::string(mode) + '/' + std::to_string(length) + '_' + err + '_' + std::to_string(end_up) + ".csv";
                        std::vector<int32_t> s_ts = get_python_wavefront_score(python_file);

                        FrontXM FrXM;
                        Front FrI, FrD;

                        std::string test_file = "paper/test/data/" + std::string(mode) + '/' + std::to_string(length) + '_' + err + '_' + std::to_string(end_up) + ".csv";

                        std::cerr << test_file << ": " << total_time << " ms\n";
                        std::ifstream fin(test_file);
                        if (!fin.is_open())
                        {
                            std::cerr << "Error: Could not open file.\n";
                            std::exit(1);
                        }

                        std::vector<int32_t>::iterator it = s_ts.begin();
                        std::string line;
                        std::getline(fin, line);
                        while (std::getline(fin, line))
                        {
                            std::string t, q;
                            std::stringstream ss(line);
                            std::getline(ss, t, ',');
                            std::getline(ss, q);

                            auto start = std::chrono::steady_clock::now();

                            auto [s_t, d_t, f_t] = lwfa(q, t, x, o, e, j, c_f, c_r, FrXM, FrI, FrD);
                            if (s_t != *it)
                            {
                                std::cerr << "score not equal to python version\n"
                                          << t << '\n'
                                          << q << '\n';
                                std::exit(1);
                            }
                            std::string aln = track(c_f, c_r, s_t, d_t, f_t, FrXM, FrI, FrD, t.size(), q.size(), x, o, e, j);

                            auto end = std::chrono::steady_clock::now();
                            std::chrono::duration<double, std::milli> elapsed = end - start;
                            total_time += elapsed.count();

                            std::reverse(aln.begin(), aln.end());
                            int32_t s_t_aln = eval_score(aln, q, t, x, o, e, j);
                            if (s_t_aln != *it)
                            {
                                std::cerr << "score recovered from alignment not equal to python version\n"
                                          << t << '\n'
                                          << q << '\n';
                                std::exit(1);
                            }
                            ++it;
                        }

                        fin.close();
                    }
                }
            }
        }
    }
    std::cerr << total_time << " ms\n";
}
