/**
 * @file    RunAction.cc
 * @brief   Implementation of the G4UserRunAction for global statistics.
 */

#include "RunAction.hh"

/*───────────────────────────── Geant4 ─────────────────────────────────*/
#include "G4AccumulableManager.hh"
#include "G4AnalysisManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4Threading.hh"
#include "G4UnitsTable.hh"

/*──────────────────────────── project ────────────────────────────────*/
// #include "HistogramManager.hh"
#include "DataLogger.hh"

/*═════════════════════════════════════════════════════════════════════*/
/*  ctor – register accumulables                                       */
/*═════════════════════════════════════════════════════════════════════*/
RunAction::RunAction(const geom::GeometryConfig &cfg,
					 util::DataLogger *logger)
	: cfg_{cfg}, logger_{logger}, nCones_{cfg.nCones()}, nPanels_{cfg.nPanels()}, coneIon_(nCones_), coneCap_(nCones_), panelIon_(nPanels_), panelCap_(nPanels_)
{
	auto *accMan = G4AccumulableManager::Instance();
	for (auto &a : coneIon_)
		accMan->RegisterAccumulable(a);
	for (auto &a : coneCap_)
		accMan->RegisterAccumulable(a);
	for (auto &a : panelIon_)
		accMan->RegisterAccumulable(a);
	for (auto &a : panelCap_)
		accMan->RegisterAccumulable(a);
}

/*═════════════════════════════════════════════════════════════════════*/
/*  BeginOfRunAction                                                   */
/*═════════════════════════════════════════════════════════════════════*/
void RunAction::BeginOfRunAction(const G4Run *)
{
	/* 1)  Informative banner */
	G4cout << "[RunAction] BeginOfRunAction on thread "
		   << G4Threading::G4GetThreadId() << G4endl;

	/* 2)  Master thread: create results/<timestamp>/ + TSV header */
	if (G4Threading::IsMasterThread())
		logger_->InitOutputFiles(cfg_);

	/* 3)  ROOT file & histograms */
	// if (G4Threading::IsMasterThread())
	// {
	//     auto* mgr = G4AnalysisManager::Instance();
	//     mgr->SetNtupleMerging(true);
	//     mgr->OpenFile("output/root/muon_ionization_output.root");
	// }
	// HistogramManager::Initialize();

	/* 4)  Reset accumulables at run start */
	G4AccumulableManager::Instance()->Reset();
}

/*═════════════════════════════════════════════════════════════════════*/
/*  EndOfRunAction                                                     */
/*═════════════════════════════════════════════════════════════════════*/
void RunAction::EndOfRunAction(const G4Run *run)
{
	/* 1) Merge all thread-local accumulables → master copy */
	G4AccumulableManager::Instance()->Merge();

	/* 2) Master writes histograms & run summary */
	if (G4Threading::IsMasterThread())
	{
		/*── 2a  Write / close ROOT histograms ─────────────────────*/
		// auto* mgr = G4AnalysisManager::Instance();
		// mgr->Write();
		// mgr->CloseFile();

		/*── 2b  Extract merged tallies ───────────────────────────*/
		std::vector<unsigned> coneIon(nCones_), coneCap(nCones_);
		std::vector<unsigned> panelIon(nPanels_), panelCap(nPanels_);

		unsigned long totalIon = 0, totalCap = 0;
		for (std::size_t i = 0; i < nCones_; ++i)
		{
			coneIon[i] = coneIon_[i].GetValue();
			coneCap[i] = coneCap_[i].GetValue();
			totalIon += coneIon[i];
			totalCap += coneCap[i];
		}
		for (std::size_t i = 0; i < nPanels_; ++i)
		{
			panelIon[i] = panelIon_[i].GetValue();
			panelCap[i] = panelCap_[i].GetValue();
		}

		/*── 2c  One-shot JSON + TSV footer via DataLogger ─────────*/
		logger_->DumpRunSummary(cfg_,
								run->GetNumberOfEvent(),
								totalIon,
								totalCap,
								coneIon, coneCap,
								panelIon, panelCap);

		/*── 2d  Print nice summary to terminal ────────────────────────────*/
		PrintRunSummary(run->GetNumberOfEvent(), totalCap, totalIon);

		G4cout << "[RunAction] EndOfRunAction completed on master.\n";
	}
}