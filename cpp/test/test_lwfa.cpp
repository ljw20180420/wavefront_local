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

                        std::string python_file = "test/py/align/pylwfa/" + score_schema_name + '/' + std::string(mode) + '/' + std::to_string(length) + '_' + err + '_' + std::to_string(end_up) + ".csv";
                        std::vector<int32_t> s_ts = get_python_wavefront_score(python_file);

                        FrontXM FrXM;
                        Front FrI, FrD;

                        std::string test_file = "test/data/" + std::string(mode) + '/' + std::to_string(length) + '_' + err + '_' + std::to_string(end_up) + ".csv";

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

                            auto start = std::chrono::high_resolution_clock::now();

                            int32_t s_t, d_t, f_t;
                            std::string aln;
                            int32_t s_t_aln;
                            if (score_schema_name == "bowtie")
                            {
                                std::tie(s_t, d_t, f_t) = lwfa<bowtie_wfa.x, bowtie_wfa.o, bowtie_wfa.e, bowtie_wfa.j>(q, t, c_f, c_r, FrXM, FrI, FrD);
                                aln = track<bowtie_wfa.x, bowtie_wfa.o, bowtie_wfa.e, bowtie_wfa.j>(c_f, c_r, s_t, d_t, f_t, FrXM, FrI, FrD, t.size(), q.size());
                                std::reverse(aln.begin(), aln.end());
                                s_t_aln = eval_score<bowtie_wfa.x, bowtie_wfa.o, bowtie_wfa.e, bowtie_wfa.j>(aln, q, t);
                            }
                            else
                            {
                                // score_schema_name == "bwa"
                                std::tie(s_t, d_t, f_t) = lwfa<bwa_wfa.x, bwa_wfa.o, bwa_wfa.e, bwa_wfa.j>(q, t, c_f, c_r, FrXM, FrI, FrD);
                                aln = track<bwa_wfa.x, bwa_wfa.o, bwa_wfa.e, bwa_wfa.j>(c_f, c_r, s_t, d_t, f_t, FrXM, FrI, FrD, t.size(), q.size());
                                std::reverse(aln.begin(), aln.end());
                                s_t_aln = eval_score<bwa_wfa.x, bwa_wfa.o, bwa_wfa.e, bwa_wfa.j>(aln, q, t);
                            }

                            auto end = std::chrono::high_resolution_clock::now();
                            std::chrono::duration<double, std::milli> elapsed = end - start;
                            total_time += elapsed.count();

                            if (s_t != *it)
                            {
                                std::cerr << "score not equal to python version\n"
                                          << t << '\n'
                                          << q << '\n';
                                std::exit(1);
                            }
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
