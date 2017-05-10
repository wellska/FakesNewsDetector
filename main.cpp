//
//  main.cpp
// 
//
//  Created by Kaitlyn Wells on 4/12/17.
//  Copyright © 2017 Kaitlyn Wells. All rights reserved.
//


#include <map>
#include <iomanip>
#include <iostream>
#include <string>
#include <cmath>
#include "csvstream.h"
#include <vector>
#include <set>
#include <cstdlib>

using namespace std;

class Classifier {
private:
    //double log_prob_score;
    map<string,int> words; //body words
    map<string, int> titles;
    //label, word, numPosts
    map<pair<string,string>, int> wordsTitles;
    vector<string> articles;
    int correct = 0;
    int count = 0;
    //number of posts in set that contain w
    
    
public:
    //default constructor
    Classifier(){}
    
    void addData(pair<string,string> row_in){
        addTitles(row_in.first);
        //adding each unique word
        istringstream source(row_in.second);
        set<string> words;
        string word;
        
        while(source >> word){
            pair<string,string> insert = {row_in.first, word};
            std::pair<std::set<string>::iterator,bool> ret;
            ret = words.insert(word);
            if(ret.second == true){
                addwordsTitles(insert);
                addWords(word);
            }
        }
        
        
        
        string concatenatedPost = "title = " + row_in.first + ", body = " + row_in.second;
        articles.push_back(concatenatedPost);
        
    }
    
    void addTest(pair<string,string> row_in){
        predictLogProbability(row_in);
    }
    
    //effects:adds individual words in map and counts number of posts with that word
    void addWords(string word){
        if (words.find(word) == words.end()){
            words.insert(std::pair<string,int>(word, 1));
            //add count to word value
        } else {
            words[word] = words[word] + 1;
        }
    }
    
    //effects:adds labels in a map and counts number of posts with label
    void addTitles(string title){
        if (titles.find(title) == titles.end()){
            titles.insert(std::pair<string,int>(title, 1));
            //add count to word value
        } else {
            titles[title] = titles[title] + 1;
        }
        return;
    }
    //effects:adds pairs to the map
    void addwordsTitles (pair<string, string> row){
        if (wordsTitles.find(row) == wordsTitles.end()){
            wordsTitles[row] = 1;
        } else {
            wordsTitles[row] = wordsTitles[row] + 1;
        }
        return;
    }
    
    //effects: returns word count for label that contained the word
    int wordCount(string title, string word){
        pair<string,string> pair_type = {title,word};
        if(wordsTitles.find(pair_type) != wordsTitles.end()){
            return wordsTitles[pair_type];
        } else {
            return 0;
        }
    }
    
    //effects: finds number of unique words in entire training set
    //returns vocabularySize
    long numUniqueVocabSet(){
        return words.size();
    }
    
    
    //effects: computes log-prior
    double compute_log_prior(string title){
        cout << setprecision(3);
        int numTrainingPostsLabel = titles[title];
        double numArticles = articles.size();
        double value = log(numTrainingPostsLabel/numArticles);
        return value;
    }
    
    //effects: computes log likelihood
    //hint: if w was never in a post with label c, use alternate formula
    double compute_log_like(pair<string,string> pair_type){
        cout << setprecision(3);
        double numArticles = articles.size();
        //pair<string,string> pair_type = {label, word};
        if (wordsTitles.find(pair_type) != wordsTitles.end()){
            double numPosts = titles[pair_type.first];
            double numTrainingTitlesWord = wordsTitles[pair_type];
            double value = log(numTrainingTitlesWord/numPosts);
            return value;
        } else if (words.find(pair_type.second) != words.end()) {
            double numTrainingWords = words[pair_type.second];
            
            double value = log(numTrainingWords / numArticles);
            return value;
        } else {
            double value = log(1/numArticles);
            return value;
        }
    }
    
    void predictLogProbability(pair<string, string> pair_type){
        cout << setprecision(3);
        //calculate log likelihood for each unique word
        string word;
        
        map<string,double> scores;
        
        for(map<string,int>::iterator it = titles.begin(); it != titles.end(); it++){
            double value = 0.00;
            istringstream source(pair_type.second);
            set<string> words;
            while(source >> word){
                std::pair<std::set<string>::iterator,bool> ret;
                ret = words.insert(word);
                if(ret.second == true){
                    pair<string,string> insert = {it->first, word};
                    value = compute_log_like(insert) + value;
                }
                
            }
            double score = compute_log_prior(it->first) + value;
            scores.insert(pair<string,double> (it->first,score));
            
        }
        double highest_score = scores.begin()->second;
        string highest_label = scores.begin()->first;
        for (map<string,double>::iterator it = scores.begin(); it != scores.end(); it++){
            double score = it->second;
            if(score > highest_score){
                highest_score = score;
                highest_label = it->first;
            }
            count++;
        }
        if (titles.find(highest_label) != titles.end()){
            //either agrees / disagrees / discusses
            cout << "related" << endl;
            correct++;
        } else{
            cout << "unrelated" << endl;
        }
    }
};

int main(int argc, char **argv) {
    cout << setprecision(3);
    
    //error checking
    if(argc != 3){
        if(argc == 4 && (string(argv[3]) != "--debug")){
            cout << "Usage: main TRAIN_FILE TEST_FILE [--debug]" << endl;
            exit(EXIT_FAILURE);
        } else {
            if(argc != 4){
                cout << "Usage: main TRAIN_FILE TEST_FILE [--debug]" << endl;
                exit(EXIT_FAILURE);
            }
        }
    }
    
    string filename = argv[1];
    string test_data = argv[2];
    Classifier trainingData;
    try{
        // Open file
        csvstream csvin(filename);
        // A row is a map<string, string>, key = column name, value = datum
        csvstream::row_type row;
        stringstream label;
        stringstream content;
        
        // Read file
        
        while (csvin >> row) {
            label << row["Headline"];
            content << row["Body ID"];
            pair<string, string> data;
            
            data = {label.str(), content.str()};
            
            //cout << data.first << " : " << data.second << endl;
            trainingData.addData(data);
            label.str("");
            content.str("");
        }
    } catch(csvstream_exception &error){
        cout << "Error opening file: " << filename << endl;
        exit(EXIT_FAILURE);
    }
    
    
    
    // Read file
    
    try{
        csvstream infile(test_data);
        csvstream::row_type input;
        stringstream label;
        stringstream content;
        while (infile >> input){
            label << input["Headline"];
            content << input["Body ID"];
            pair<string,string> data;
            
            data = {label.str(), content.str()};
            trainingData.addTest(data);
            label.str("");
            content.str("");
        }
    } catch(csvstream_exception &error){
        cout << "Error opening file: " << filename << endl;
        exit(EXIT_FAILURE);
    }
    
    return 0;
    
}



