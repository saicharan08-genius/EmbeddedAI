/*
* Copyright (C) 2017 Boris Altshul.
* All rights reserved.
*
* The software in this package is published under the terms of the BSD
* style license a copy of which has been included with this distribution in
* the LICENSE.txt file.
*/

#include <factor.h>
#include <Factories.h>
#include <json/json.h>
#include <fstream>
#include <gtest/gtest.h>


using namespace bayeslib;

/** \file
 \ingroup largeModel
 \{

 Large describe the network shown on picture below. <br>
 Traffic is sent over links 1,2,3,4. Inside the network traffic on each link 
 is split into 2 sublink. On the Endpoint Node the packet drops are measured
 and attributed to each of 8 sublinks 1_1, 1_2, 2_1... 4_2. Also accumulated 
 over medium interval time traffic drop is calculated for the same 8 sublinks.
 Traffic dropped can be caused by congestion on links 1..4, any of 8 sublinks or
 Endpoint Node itself.
 /// \image html "../../docs/netdiag.png" width=60%

*/




/**  Create Model of Carrier Network 
     packet drops are measured for each sublink
     The system can sample instantenues and accumulated rate of packet drop
     digitized into two regions -- hi drop and low drop
     /// \image html "../../docs/netgraph.png" width=60%
*/

int InitLargeTest(VarDb &db, FactorSet &fs)
{
   char sz[10];

   // 4 links links with 2 sublinks each 
   for (int nLink = 1; nLink <= 4; nLink++)
   {
      // conjested link

      snprintf(sz, 10, "cjl%d", nLink);
      db.AddVar(sz);

      // conjested sublink
      snprintf(sz, 10, "cjl%d_1", nLink);
      db.AddVar(sz);

      snprintf(sz, 10, "cjl%d_2", nLink);
      db.AddVar(sz);

      // packet drops

      // hi drop sublink1
      snprintf(sz, 10, "drhi%d_1", nLink);
      db.AddVar(sz);

      // hi drop sublink 2
      snprintf(sz, 10, "drhi%d_2", nLink);
      db.AddVar(sz);

      // lo drop sublink 1
      snprintf(sz, 10, "drlo%d_1", nLink);
      db.AddVar(sz);

      // lo drop sublink 2
      snprintf(sz, 10, "drlo%d_2", nLink);
      db.AddVar(sz);


      // acc hi drop sublink1
      snprintf(sz, 10, "drhia%d_1", nLink);
      db.AddVar(sz);

      // acc hi drop sublink 2
      snprintf(sz, 10, "drhia%d_2", nLink);
      db.AddVar(sz);

      // acc lo drop sublink 1
      snprintf(sz, 10, "drloa%d_1", nLink);
      db.AddVar(sz);

      // acc lo drop sublink 2
      snprintf(sz, 10, "drloa%d_2", nLink);
      db.AddVar(sz);
   }
   // conjested endpoint
   db.AddVar("cjE");

   for (int nLink = 1; nLink <= 4; nLink++)
   {
      // toplevel varaibles.
      VarSet vsConjL(db);
      // conjested link
      snprintf(sz, 10, "cjl%d", nLink);
      vsConjL << db[sz];
      std::shared_ptr<Factor> fConjL = std::make_shared<Factor>(vsConjL, db[sz]);
      fConjL->AddInstance(0, 0.95F);
      fConjL->AddInstance(1, 0.05F);

      VarSet vsConjSL1(db);
      // conjested sub link 1 
      snprintf(sz, 10, "cjl%d_1", nLink);
      vsConjSL1 << db[sz];
      std::shared_ptr<Factor> fConjSL1 = std::make_shared<Factor>(vsConjSL1, db[sz]);
      fConjSL1->AddInstance(0, 0.9F);
      fConjSL1->AddInstance(1, 0.1F);

      VarSet vsConjSL2(db);
      // conjested sub link 2 
      snprintf(sz, 10, "cjl%d_2", nLink);
      vsConjSL2 << db[sz];
      std::shared_ptr<Factor> fConjSL2 = std::make_shared<Factor>(vsConjSL2, db[sz]);
      fConjSL2->AddInstance(0, 0.9F);
      fConjSL2->AddInstance(1, 0.1F);

      // second level variables

      VarSet vsDropLo1(db);
      // low drop link 1
      VarId headVar;
      snprintf(sz, 10, "cjl%d", nLink);
      vsDropLo1 << db[sz];
      snprintf(sz, 10, "cjl%d_1", nLink);
      vsDropLo1 << db[sz];
      vsDropLo1 << db["cjE"];
      snprintf(sz, 10, "drlo%d_1", nLink);
      headVar = db[sz];
      vsDropLo1 << headVar;

      // construct factor
      std::shared_ptr<Factor> fDropLo1 = std::make_shared<Factor>(vsDropLo1, headVar);
      // load factor using factor loader
      Factor::FactorLoader flDropLo1(fDropLo1);
      flDropLo1 << 0.99F << 0.7F << 0.7F << 0.6F << 0.7F << 0.6F << 0.6F << 0.55F;
      flDropLo1 << 0.01F << 0.3F << 0.3F << 0.4F << 0.3F << 0.4F << 0.4F << 0.45F;
      /////

      VarSet vsDropLo2(db);
      // low drop link 2
      snprintf(sz, 10, "cjl%d", nLink);
      vsDropLo2 << db[sz];
      snprintf(sz, 10, "cjl%d_2", nLink);
      vsDropLo2 << db[sz];
      vsDropLo2 << db["cjE"];
      snprintf(sz, 10, "drlo%d_2", nLink);
      headVar = db[sz];
      vsDropLo2 << headVar;

      // construct factor
      std::shared_ptr<Factor> fDropLo2 = std::make_shared<Factor>(vsDropLo2, headVar);
      // load factor using factor loader
      Factor::FactorLoader flDropLo2(fDropLo2);
      flDropLo2 << 0.99F << 0.7F << 0.7F << 0.6F << 0.7F << 0.6F << 0.6F << 0.55F;
      flDropLo2 << 0.01F << 0.3F << 0.3F << 0.4F << 0.3F << 0.4F << 0.4F << 0.45F;

      //HHHHHHHHHHHHHHH
      VarSet vsDropHi1(db);
      // low drop link 1
      snprintf(sz, 10, "cjl%d", nLink);
      vsDropHi1 << db[sz];
      snprintf(sz, 10, "cjl%d_1", nLink);
      vsDropHi1 << db[sz];
      vsDropHi1 << db["cjE"];
      snprintf(sz, 10, "drhi%d_1", nLink);
      headVar = db[sz];
      vsDropHi1 << headVar;

      // construct factor
      std::shared_ptr<Factor> fDropHi1 = std::make_shared<Factor>(vsDropHi1, headVar);
      // load factor using factor loader
      Factor::FactorLoader flDropHi1(fDropHi1);
      flDropHi1 << 0.998F << 0.9F << 0.9F << 0.8F << 0.9F << 0.7F << 0.7F << 0.65F;
      flDropHi1 << 0.002F << 0.1F << 0.1F << 0.2F << 0.1F << 0.3F << 0.3F << 0.35F;
      /////

      VarSet vsDropHi2(db);
      // low drop link 2
      snprintf(sz, 10, "cjl%d", nLink);
      vsDropHi2 << db[sz];
      snprintf(sz, 10, "cjl%d_2", nLink);
      vsDropHi2 << db[sz];
      vsDropHi2 << db["cjE"];
      snprintf(sz, 10, "drhi%d_2", nLink);
      headVar = db[sz];
      vsDropHi2 << headVar;

      // construct factor
      std::shared_ptr<Factor> fDropHi2 = std::make_shared<Factor>(vsDropHi2, headVar);
      // load factor using factor loader
      Factor::FactorLoader flDropHi2(fDropHi2);

      flDropHi2 << 0.998F << 0.9F << 0.9F << 0.8F << 0.9F << 0.7F << 0.7F << 0.65F;
      flDropHi2 << 0.002F << 0.1F << 0.1F << 0.2F << 0.1F << 0.3F << 0.3F << 0.35F;

      ////////////////////////////////////////////////////
      // ACCUMULATED drop VARS

      VarSet vsDropLoA1(db);
      // low drop link 1
      snprintf(sz, 10, "cjl%d", nLink);
      vsDropLoA1 << db[sz];
      snprintf(sz, 10, "cjl%d_1", nLink);
      vsDropLoA1 << db[sz];
      vsDropLoA1 << db["cjE"];
      snprintf(sz, 10, "drloa%d_1", nLink);
      headVar = db[sz];
      vsDropLoA1 << headVar;

      // construct factor
      std::shared_ptr<Factor> fDropLoA1 = std::make_shared<Factor>(vsDropLoA1, headVar);
      // load factor using factor loader
      Factor::FactorLoader flDropLoA1(fDropLoA1);
      flDropLoA1 << 0.95F << 0.5F << 0.5F << 0.4F << 0.4F << 0.4F << 0.4F << 0.30F;
      flDropLoA1 << 0.05F << 0.5F << 0.5F << 0.6F << 0.6F << 0.6F << 0.6F << 0.70F;
      /////

      VarSet vsDropLoA2(db);
      // low drop link 2
      snprintf(sz, 10, "cjl%d", nLink);
      vsDropLoA2 << db[sz];
      snprintf(sz, 10, "cjl%d_2", nLink);
      vsDropLoA2 << db[sz];
      vsDropLoA2 << db["cjE"];
      snprintf(sz, 10, "drloa%d_2", nLink);
      headVar = db[sz];
      vsDropLoA2 << headVar;

      // construct factor
      std::shared_ptr<Factor> fDropLoA2 = std::make_shared<Factor>(vsDropLoA2, headVar);
      // load factor using factor loader
      Factor::FactorLoader flDropLoA2(fDropLoA2);
      flDropLoA2 << 0.95F << 0.5F << 0.5F << 0.4F << 0.4F << 0.4F << 0.4F << 0.30F;
      flDropLoA2 << 0.05F << 0.5F << 0.5F << 0.6F << 0.6F << 0.6F << 0.6F << 0.70F;

      //HHHHHHHHHHHHHHH
      VarSet vsDropHiA1(db);
      // low drop link 1
      snprintf(sz, 10, "cjl%d", nLink);
      vsDropHiA1 << db[sz];
      snprintf(sz, 10, "cjl%d_1", nLink);
      vsDropHiA1 << db[sz];
      vsDropHiA1 << db["cjE"];
      snprintf(sz, 10, "drhia%d_1", nLink);
      headVar = db[sz];
      vsDropHiA1 << headVar;

      // construct factor
      std::shared_ptr<Factor> fDropHiA1 = std::make_shared<Factor>(vsDropHiA1, headVar);
      // load factor using factor loader
      Factor::FactorLoader flDropHiA1(fDropHiA1);
      flDropHiA1 << 0.999F << 0.8F << 0.7F << 0.7F << 0.7F << 0.6F << 0.7F << 0.55F;
      flDropHiA1 << 0.001F << 0.2F << 0.3F << 0.3F << 0.3F << 0.4F << 0.3F << 0.45F;
      /////

      VarSet vsDropHiA2(db);
      // low drop link 2
      snprintf(sz, 10, "cjl%d", nLink);
      vsDropHiA2 << db[sz];
      snprintf(sz, 10, "cjl%d_2", nLink);
      vsDropHiA2 << db[sz];
      vsDropHiA2 << db["cjE"];
      snprintf(sz, 10, "drhia%d_2", nLink);
      headVar = db[sz];
      vsDropHiA2 << headVar;

      // construct factor
      std::shared_ptr<Factor> fDropHiA2 = std::make_shared<Factor>(vsDropHiA2, headVar);
      // load factor using factor loader
      Factor::FactorLoader flDropHiA2(fDropHiA2);

      flDropHiA2 << 0.999F << 0.8F << 0.7F << 0.7F << 0.7F << 0.6F << 0.7F << 0.55F;
      flDropHiA2 << 0.001F << 0.2F << 0.3F << 0.3F << 0.3F << 0.4F << 0.3F << 0.45F;

      // add all Factors to the factorset
      fs.AddFactor(fConjL);
      fs.AddFactor(fConjSL1);
      fs.AddFactor(fConjSL2);
      fs.AddFactor(fDropLo1);
      fs.AddFactor(fDropLo2);
      fs.AddFactor(fDropHi1);
      fs.AddFactor(fDropHi2);
      fs.AddFactor(fDropLoA1);
      fs.AddFactor(fDropLoA2);
      fs.AddFactor(fDropHiA1);
      fs.AddFactor(fDropHiA2);
   }

   VarSet vsConjE(db);
   // conjested link
   vsConjE << db["cjE"];
   std::shared_ptr<Factor> fConjE = std::make_shared<Factor>(vsConjE, db["cjE"]);
   fConjE->AddInstance(0, 0.92F);
   fConjE->AddInstance(1, 0.08F);
   fs.AddFactor(fConjE);


   return 0;
}


/**  Evaluate  information about packet drops on on certain segents of network 
     infer most likely congested segments on the network
*/
int LargeTest1()
{
   VarDb db;
   FactorSet fs(db);
   InitLargeTest(db, fs);

   std::string s = fs.GetJson(db);
   printf("\n==Large test with edge Pruning==\n%s\n", s.c_str());


   VarSet vsSample(db), vsSolve(db);
   vsSolve << db["cjl1_1"] << db["cjl1_2"] << db["cjl1"];
   vsSolve << db["cjl2_1"] << db["cjl2_2"] << db["cjl2"];
   vsSolve << db["cjl3_1"] << db["cjl3_2"] << db["cjl3"];
   vsSolve << db["cjl4_1"] << db["cjl4_2"] << db["cjl4"];
   vsSolve << db["cjE"];

   VarSet varDbSet = db.GetVarSet();



   vsSample = db.GetVarSet().Substract(vsSolve);

   s = vsSample.GetJson(db);
   printf("\n   == Test Sample Varset ==\n%s\n", s.c_str());



   Clause cSample(vsSample);

#if 0
   cSample.SetVar(db["drhi3_1"], true);
   cSample.SetVar(db["drlo3_1"], true);
   cSample.SetVar(db["drhi3_2"], false);
   cSample.SetVar(db["drlo3_2"], true);
   cSample.SetVar(db["drhia3_1"], false);
   cSample.SetVar(db["drloa3_1"], true);
   cSample.SetVar(db["drhia3_2"], true);
   cSample.SetVar(db["drhia3_2"], false);
#endif

   cSample.SetVar(db["drhi3_1"], true);
   cSample.SetVar(db["drlo3_1"], true);
   cSample.SetVar(db["drhi3_2"], false);
   cSample.SetVar(db["drlo3_2"], false);
   cSample.SetVar(db["drhia3_1"], false);
   cSample.SetVar(db["drloa3_1"], true);
   cSample.SetVar(db["drhia3_2"], false);
   cSample.SetVar(db["drhia3_2"], false);


   s = cSample.GetJson(db);
   printf("\n   == Test Sample Clause ==\n%s\n", s.c_str());

   fs.PruneEdges(cSample);
   // s = fs.GetJson();
   // printf("\n =After Prune Edges =\n%s\n", s.c_str());


   //printf("\n== Sample = %s\n", cSample.GetJson().c_str());
   fs.ApplyClause(cSample);
   s = fs.GetJson(db);
   printf("\n =After apply =\n%s\n", s.c_str());

   fs.MaximizeVar(varDbSet);

   s = fs.GetJson(db);
   printf("\n =After MAX up =\n%s\n", s.c_str());


   std::shared_ptr<Factor> res1 = fs.Merge();
   s = res1->GetJson(db);
   printf("\n==After MPE calculation ==\n%s\n", s.c_str());

   VarSet vsMpe = res1->GetExtendedVarSet();
   InstanceId instanceMpe = res1->GetExtendedClause(0);
   Clause clMpe(vsMpe, instanceMpe);
   s = clMpe.GetJson(db);
   printf("==MPE clause ==\n%s\n", s.c_str());

   for (VarId vid = vsMpe.GetFirst(); vid != 0; vid = vsMpe.GetNext(vid))
   {
      if (vid == db["drlo3_1"] ||
         vid == db["drhi3_1"] ||
         vid == db["drloa3_1"] || vid == db["cjl3_1"] )
      {
         EXPECT_TRUE(clMpe[vid]);
      }
      else
      {
         EXPECT_FALSE(clMpe[vid]);
      }
   }
   EXPECT_NEAR(0.00024, res1->Get(0), 0.00001);


   return 0;
}

/**
   Same test as LargeTest1() but performs algorithm optimization
   that dramatically reduces duration of calculations
  
*/

int LargeTest2()
{
   VarDb db;
   FactorSet fs(db);
   InitLargeTest(db, fs);

   std::string s = fs.GetJson(db);
   printf("\n==Large test2  with edge Pruning==\n%s\n", s.c_str());


   VarSet vsSample(db), vsSolve(db);
   vsSolve << db["cjl1_1"] << db["cjl1_2"] << db["cjl1"];
   vsSolve << db["cjl2_1"] << db["cjl2_2"] << db["cjl2"];
   vsSolve << db["cjl3_1"] << db["cjl3_2"] << db["cjl3"];
   vsSolve << db["cjl4_1"] << db["cjl4_2"] << db["cjl4"];
   vsSolve << db["cjE"];

   VarSet varDbSet = db.GetVarSet();



   vsSample = db.GetVarSet().Substract(vsSolve);

   s = vsSample.GetJson(db);
   printf("\n   == Test Sample Varset ==\n%s\n", s.c_str());



   Clause cSample(vsSample);

#if 0
   cSample.SetVar(db["drhi3_1"], true);
   cSample.SetVar(db["drlo3_1"], true);
   cSample.SetVar(db["drhi3_2"], false);
   cSample.SetVar(db["drlo3_2"], true);
   cSample.SetVar(db["drhia3_1"], false);
   cSample.SetVar(db["drloa3_1"], true);
   cSample.SetVar(db["drhia3_2"], true);
   cSample.SetVar(db["drhia3_2"], false);
#endif

   cSample.SetVar(db["drhi3_1"], true);
   cSample.SetVar(db["drlo3_1"], true);
   cSample.SetVar(db["drhi3_2"], false);
   cSample.SetVar(db["drlo3_2"], false);
   cSample.SetVar(db["drhia3_1"], false);
   cSample.SetVar(db["drloa3_1"], true);
   cSample.SetVar(db["drhia3_2"], false);
   cSample.SetVar(db["drhia3_2"], false);


   s = cSample.GetJson(db);
   printf("\n   == Test Sample Clause ==\n%s\n", s.c_str());

   fs.PruneEdges(cSample);
   // s = fs.GetJson();
   // printf("\n =After Prune Edges =\n%s\n", s.c_str());


   //printf("\n== Sample = %s\n", cSample.GetJson().c_str());
   fs.ApplyClause(cSample);
   //s = fs.GetJson(db);
   //printf("\n =After apply =\n%s\n", s.c_str());

   s = varDbSet.GetJson(db);
   printf("\n==== Initial VS%s", s.c_str());


   InteractionGraph ig(&fs);
   VarSet optVs = ig.GetElimOrder();
   s = optVs.GetJson(db);
   printf("\n==== Updated VS%s", s.c_str());

   fs.MaximizeVar(optVs);


   s = fs.GetJson(db);
   printf("\n =After MAX up =\n%s\n", s.c_str());


   std::shared_ptr<Factor> res1 = fs.Merge();
   s = res1->GetJson(db);
   printf("\n==After MPE calculation ==\n%s\n", s.c_str());

   VarSet vsMpe = res1->GetExtendedVarSet();
   InstanceId instanceMpe = res1->GetExtendedClause(0);
   Clause clMpe(vsMpe, instanceMpe);
   s = clMpe.GetJson(db);
   printf("==MPE clause ==\n%s\n", s.c_str());

   for (VarId vid = vsMpe.GetFirst(); vid != 0; vid = vsMpe.GetNext(vid))
   {
      if (vid == db["drlo3_1"] ||
         vid == db["drhi3_1"] ||
         vid == db["drloa3_1"] || vid == db["cjl3_1"])
      {
         EXPECT_TRUE(clMpe[vid]);
      }
      else
      {
         EXPECT_FALSE(clMpe[vid]);
      }
   }
   EXPECT_NEAR(0.00024, res1->Get(0), 0.00001);

   return 0;
}


#if 0
float CalcDropProbability(VarState dropState, VarState stage1, ValState stage2, ValState stage3 )
{
   float fPassRate = 1.0F;
   VarState ar[] = {stage1, stage2, stage3};

   for (auto st: ar)
   {
      switch(st)
      {
         case 0: fPassRate *= 0.995; break;
         case 1: fPassRate *= 0.95; break;
         case 2: fPassRate *= 0.8; break;
         case 3: fPassRate = 0; break;
         default: break;
      }
   }

   float expectedPassHi = 1.0F;
   float expectedPassLo = 1.0F;

   switch(dropState)
   {
      case 0:   // expected pass rate is 0.98 - 1.0

         expectedPassHi = 1.0F;
         expectedPassLo = 0.98F;
         break;

      case 1:  // expected pass rate 0.8 - 0.98
         expectedPassHi = 0.8F;
         expectedPassLo = 0.90F;
         break;

      case 2: // expected pass rate 0.01 - 0.8
         expectedPassHi = 0.8F;
         expectedPassLo = 0.01F;
         break;

      case 3:
         expectedPassHi = 0.F;
         expectedPassLo = 0.0F;
         break;
   }

   float fRes1 = expectedPassHi - fPassRate ;
   float fRes2 = fPassRate - expectedPassLo;

   return 1;

}
#endif


/** calculate Droplevel probability 
    @param dropLevel drop level for which probalility is calculated 0-4
    @param  cj[3]  conjestion levels of the components of  the network path 
    @return drop level probability
 */    

static float
CalcDropLevelProbability(int dropLevel, std::initializer_list<int> cj)
{
   float passLevel = 1.;
   for(auto iter : cj)
   {
      cjLevel = *iter;
      // calculate packet drop based on congestion;
      switch(cjLevel)
      {
      case 0:   // no congesion
         passl *= 0.99;
         break;
      case 1:   // low congestion   
         passl *= 0.95;
         break;
      case 2:   // high congestion
         passl *=0.8;
         break;
      case 3:   // full drop
         passl = 0;
      }
   }

   // drop per 100 samples
   int nDropRate = 100-100*passl;
   int nDropDetectedMin = 0;
   int nDropDetectedMax = 1;
   
   switch(dropLevel)
   {
   case 0:  // no drop

      dropDetectedMin = 0;
      dropDetectedMax = 2;
      break;

   case 1:  // low drop
      dropDetectedMin = 3;
      dropDetectedMax = 8;
      break;

   case 2:  // highdrop
      dropDetectedMin = 8;
      dropDetectedMax = 20;
      break;

      case 3:
      dropDetectedMin = 21;
      dropDetectedMax = 100;
      break;
   }

   // will use poisson multiplication
   auto poisson = [](int k, int lambda) -> float {
      float res=exp(-lambda);
      
      for(int i = 1; i <= k; i++)
      {
         res *= (lambda /i) ;
      }
      return res;
   }
   
   float prob = 0;
   for(int drop = dropDetectedMin; drop <= dropDetectedMax; drop++)
   {
      prob += poisson(drop, nDropRate);
   }
   
   return prob;
}


/**  Create Model of Carrier Network
     packet drops are measured for each sublink
     The system can sample instantenues and accumulated rate of packet drop
     digitized into two regions -- hi drop and low drop
     /// \image html "../../docs/netgraph.png" width=60%
*/

int InitLargeTest2(VarDb &db, FactorSet &fs)
{
   char sz[10];

   // 4 links links with 2 sublinks each
   for (int nLink = 1; nLink <= 3; nLink++)
   {
      // conjested link

      snprintf(sz, 10, "cj%d", nLink);
      // no congestion, low, high, full drop
      db.AddVar(sz, {"none", "1", "2", "full"});

      // conjested sublink
      snprintf(sz, 10, "cj%d_1", nLink);
      db.AddVar(sz,{"none", "1", "2", "full"});

      snprintf(sz, 10, "cj%d_2", nLink);
      db.AddVar(sz,{"none", "1", "2", "full"});

      // packet drops

      // drop sublink1
      snprintf(sz, 10, "dr%d_1", nLink);
      db.AddVar(sz,{"none", "1", "2", "full"});

      // drop sublink 2
      snprintf(sz, 10, "dr%d_2", nLink);
      db.AddVar(sz,{"none", "1", "2", "full"});


      // acc drop sublink1
      snprintf(sz, 10, "dra%d_1", nLink);
      db.AddVar(sz,{"none", "1", "2", "full"});

      // acc drop sublink 2
      snprintf(sz, 10, "dra%d_2", nLink);
      db.AddVar(sz,{"none", "1", "2", "full"});

   }
   // conjested endpoint
   db.AddVar("cjE", {"none", "1", "2", "full"});

   for (int nLink = 1; nLink <= 3; nLink++)
   {
      // toplevel varaibles.
      VarSet vsConj(db);
      // conjested link
      snprintf(sz, 10, "cj%d", nLink);
      vsConj << db[sz];
      std::shared_ptr<Factor> fConj = std::make_shared<Factor>(vsConj, db[sz]);
      *fConj << 0.95F << 0.03F << 0.01F << 0.01F;


      VarSet vsConjSL1(db);
      // conjested sub link 1
      snprintf(sz, 10, "cj%d_1", nLink);
      vsConjSL1 << db[sz];
      std::shared_ptr<Factor> fConjSL1 = std::make_shared<Factor>(vsConjSL1, db[sz]);
      *fConjSL1 << 0.95F << 0.03F << 0.01F << 0.01F;

      VarSet vsConjSL2(db);
      // conjested sub link 2
      snprintf(sz, 10, "cj%d_2", nLink);
      vsConjSL2 << db[sz];
      std::shared_ptr<Factor> fConjSL2 = std::make_shared<Factor>(vsConjSL2, db[sz]);
      *fConjSL1 << 0.95F << 0.03F << 0.01F << 0.01F;

      // second level variables

      VarSet vsDrop1(db);
      //  drop link 1
      VarId headVar;
      snprintf(sz, 10, "cj%d", nLink);
      vsDrop1 << db[sz];
      snprintf(sz, 10, "cj%d_1", nLink);
      vsDrop1 << db[sz];
      vsDrop1 << db["cjE"];
      snprintf(sz, 10, "dr%d_1", nLink);
      headVar = db[sz];
      vsDrop1 << headVar;

      // construct factor
      std::shared_ptr<Factor> fDrop1 = std::make_shared<Factor>(vsDrop1, headVar);
      // load factor using factor loader
      Factor::FactorLoader flDrop1(fDrop1);

      // flDrop1 << CalcDropProbability();

      // variables parent variables of drop node
      int cjLink, cjSublink, cjEndpoint;
      int dropLevel;  // values of leaf variable

      for(dropLevel = 0; dropLevel < 4; dropLevel++)
      {
         for(cjEndpoint = 0; cjEndpoint < 4; cjEndpoint++)
         {
            for(cjSublink = 0; cjSublink < 4; cjSublink++)
            {
               for(cjLink = 0; cjLink < 4; cjLink++)
               {
                  flDrop1 <<  CalcDropLevelProbability(dropLevel, cjEndpoint, cjSublink, cjLink);
               }

            }

         }
      }

      // same for sublink 2 on link nLink
      VarSet vsDrop2(db);
      // low drop link 2
      snprintf(sz, 10, "cj%d", nLink);
      vsDrop2 << db[sz];
      snprintf(sz, 10, "cj%d_2", nLink);
      vsDrop2 << db[sz];
      vsDrop2 << db["cjE"];
      snprintf(sz, 10, "dr%d_2", nLink);
      headVar = db[sz];
      vsDrop2 << headVar;

      // construct factor
      std::shared_ptr<Factor> fDrop2 = std::make_shared<Factor>(vsDrop2, headVar);
      // load factor using factor loader
      Factor::FactorLoader flDrop2(fDrop2);

      for(dropLevel = 0; dropLevel < 4; dropLevel++)
      {
         for(cjEndpoint = 0; cjEndpoint < 4; cjEndpoint++)
         {
            for(cjSublink = 0; cjSublink < 4; cjSublink++)
            {
               for(cjLink = 0; cjLink < 4; cjLink++)
               {
                  flDrop2 <<  CalcDropLevelProbability(dropLevel, cjEndpoint, cjSublink, cjLink);
               }

            }

         }
      }


      ////////////////////////////////////////////////////
      // ACCUMULATED drop VARS

      VarSet vsDropA1(db);
      // accumulated drop sublink link 1
      snprintf(sz, 10, "cj%d", nLink);
      vsDropA1 << db[sz];
      snprintf(sz, 10, "cj%d_1", nLink);
      vsDropA1 << db[sz];
      vsDropA1 << db["cjE"];
      snprintf(sz, 10, "dra%d_1", nLink);
      headVar = db[sz];
      vsDropA1 << headVar;

      // construct factor
      std::shared_ptr<Factor> fDropA1 = std::make_shared<Factor>(vsDropA1, headVar);
      // load factor using factor loader
      Factor::FactorLoader flDropLoA1(fDropLoA1);

      
      
      
      /////

      VarSet vsDropA2(db);
      // low drop link 2
      snprintf(sz, 10, "cj%d", nLink);
      vsDropA2 << db[sz];
      snprintf(sz, 10, "cj%d_2", nLink);
      vsDropA2 << db[sz];
      vsDropA2 << db["cjE"];
      snprintf(sz, 10, "dra%d_2", nLink);
      headVar = db[sz];
      vsDropA2 << headVar;

      // construct factor
      std::shared_ptr<Factor> fDropA2 = std::make_shared<Factor>(vsDropA2, headVar);
      // load factor using factor loader
      Factor::FactorLoader flDropA2(fDropA2);
      flDropLoA2 << 0.95F << 0.5F << 0.1F << 0.0F;
      flDropLoA2 << 0.5F << 0.4F << 0.08F << 0.0F;
      flDropLoA2 << 0.08F << 0.07F << 0.05F << 0.0F;
      flDropLoA2 << 0.0F << 0.0F << 0.0F << 0.0F;




      // add all Factors to the factorset
      fs.AddFactor(fConjL);
      fs.AddFactor(fConjSL1);
      fs.AddFactor(fConjSL2);
      fs.AddFactor(fDropLo1);
      fs.AddFactor(fDropLo2);
      fs.AddFactor(fDropHi1);
      fs.AddFactor(fDropHi2);
      fs.AddFactor(fDropLoA1);
      fs.AddFactor(fDropLoA2);
      fs.AddFactor(fDropHiA1);
      fs.AddFactor(fDropHiA2);
   }

   VarSet vsConjE(db);
   // conjested link
   vsConjE << db["cjE"];
   std::shared_ptr<Factor> fConjE = std::make_shared<Factor>(vsConjE, db["cjE"]);
   fConjE->AddInstance(0, 0.92F);
   fConjE->AddInstance(1, 0.08F);
   fs.AddFactor(fConjE);


   return 0;
}


